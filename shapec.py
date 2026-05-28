from __future__ import annotations

from dataclasses import dataclass
from itertools import chain, cycle
import re
from typing import Iterator

from PIL import Image
from pycparser import c_lexer


@dataclass
class Token:
    value: str
    preproc: str | None = None
    preproc_pos: int | None = None

    def space_needed_before(self, t2: Token):
        if (
            (self.value[-1].isalnum() or self.value[-1] == '_')
            and (t2.value[0].isalnum() or t2.value[0] == '_')
        ):
            return True
        if (
            self.preproc == t2.preproc == 'define_const'
            and self.preproc_pos == 2
            and t2.value == '('
        ):
            return True
        return False


class CCodeShaper:
    def __init__(self, source: str, comment_fill='_'):
        # Normalize the source
        source = re.sub(r"/\*.*?\*/", " ", source, flags=re.DOTALL)  # Remove C style comments
        source = re.sub(r"//[^\n]*", "", source)  # Remove C++ style comments
        source = re.sub(r"\\\n", "", source)  # Join line continuations
        # Tokenize the source
        self.toks = self._lex_code(source)
        self.comment_fill = cycle(comment_fill)

    def _lex_preproc_cmd(self, line: str) -> list[Token]:
        line = line.strip()[1:].lstrip()
        parts = line.split(None, 1)
        if len(parts) == 1:
            return [Token('#', line, 0), Token(line, line, 1)]
        
        instr, args = parts
        result = [Token('#', instr, 0), Token(instr, instr, 1)]

        if instr == 'include':
            result += [Token(args, instr, 2)]

        elif instr == 'define':
            instr = 'define_const'
            if ' ' not in args and '(' not in args:
                # #define CONST
                result += [Token(args, instr, 2)]
            else:
                group = args.split()
                if len(group) == 2 and '(' in group[0]:
                    # #define MACRO(...) ...
                    instr = 'define_macro'
                    
                toks = self._lex_code(args)
                for i, tok in enumerate(toks, 2):
                    tok.preproc = instr
                    tok.preproc_pos = i
                result += toks

        elif instr in ['ifdef', 'ifndef', 'line', 'file']:
            result += [Token(args, instr, 2)]

        elif instr in ['if', 'elif', 'pragma']:
            result += [
                Token(tok.value, instr, i)
                for i, tok in enumerate(self._lex_code(args), 3)
            ]

        else:
            raise NotImplementedError(f"Preprocessor instruction not supported: {instr!r}")
        
        return result

    def _lex_code(self, source: str) -> list[Token]:
        toks: list[Token] = []

        for line in source.splitlines():
            line = line.strip()
            if line.startswith('#'):
                toks.extend(self._lex_preproc_cmd(line))
            else:
                # Ignore all the errors
                lexer = c_lexer.CLexer(
                    lambda a, b, c: None,
                    lambda: None,
                    lambda: None,
                    lambda a: True,
                )
                lexer.input(line)
                while (token := lexer.token()):
                    toks.append(Token(token.value))

        return toks
    
    def fit_pattern(self, pat: Pattern, threshold=100) -> str | None:
        rows = len(pat.grid)
        cols = len(pat.grid[0]) * 2  # each pattern column = two character cells
        grid = [[' ' for _ in range(cols)] for _ in range(rows)]

        # Build contiguous horizontal segments for each row
        segments: list[tuple[int, int, int]] = []  # (row, left, right) inclusive
        for i, row in enumerate(pat.grid):
            cols_in_row = [j*2 + k for j, c in enumerate(row) for k in range(2) if c >= threshold]
            if not cols_in_row:
                continue
            start = cols_in_row[0]
            end = start
            for c in cols_in_row[1:]:
                if c == end + 1:
                    # Skip all the consecutive columns
                    end = c
                    continue
                segments.append((i, start, end))
                start = end = c
            segments.append((i, start, end))

        it = iter(self.toks)
        prev_i = -1
        comment_row = -1

        for seg_idx, (i, left, right) in enumerate(segments):
            row = grid[i]

            # Whether this segment is the first one in the row
            header_segment = (i != prev_i)
            if header_segment:
                prev_i = i

            # Whether this segment is the last one in the row
            trailer_segment = (seg_idx == len(segments) - 1 or segments[seg_idx + 1][0] != i)

            # If the current row is marked as comment, fill the segment with comment
            if comment_row == i:
                self._fill_with_comment(row, left, right - left + 1, trailer_segment)
                continue

            pos = left
            prev_tok = Token(" ")  # Dummy, never needs a leading space

            while pos <= right:
                remaining = right - pos + 1
                try:
                    tok = next(it)
                except StopIteration:
                    # No more tokens, fill the rest with comments
                    self._fill_with_comment(row, pos, remaining, trailer_segment)
                    break

                # Whether the previous token was a preprocessor token
                was_preproc = prev_tok.preproc is not None
                
                if prev_tok.space_needed_before(tok):
                    # Insert a space
                    tok = Token(" " + tok.value, tok.preproc, tok.preproc_pos)
                prev_tok = tok

                tokval = tok.value
                toklen = len(tokval)
                is_preproc = tok.preproc is not None
                is_string = tokval.startswith('"') and tokval.endswith('"')

                if was_preproc and not is_preproc:
                    # Ensure normal tokens do not follow preprocessor instructions
                    comment_row = i
                    it = self._abort_segment(tok, it, row, pos, remaining, trailer_segment)
                    break

                if toklen <= remaining:  # Still has space
                    if (
                        # We are encountering preprocessor instructions
                        is_preproc and tokval == '#'
                        and (
                            # whether it is not in the beginning of this segment
                            remaining != right - left + 1
                            # or this segment is not the first one in the row
                            or not header_segment
                        )
                    ):
                        # Fill the rest of the row with comment, head to the next row
                        comment_row = i
                        it = self._abort_segment(tok, it, row, pos, remaining, trailer_segment)
                        break
                    
                    if (is_preproc and toklen == remaining and trailer_segment):
                        it = self._abort_segment(tok, it, row, pos, remaining, trailer_segment, True)
                        break
                    
                    pos = self._place_token(tok, row, pos)
                    continue
                
                # Not enough space
                if (tok.preproc is not None and tok.preproc.startswith("define") and is_string):
                    # String token in #define
                    res = self._split_string_token(tok, remaining - 1)
                    if res is None:
                        it = self._abort_segment(tok, it, row, pos, remaining, trailer_segment, True)
                        break
                    part1, part2 = res
                    pos = self._place_token(part1, row, pos)
                    it = self._abort_segment(part2, it, row, pos, 1, trailer_segment, True)

                elif (is_preproc and trailer_segment):
                    # Preprocessor token at the end of the row
                    it = self._abort_segment(tok, it, row, pos, remaining, trailer_segment, True)

                elif (not is_preproc and is_string):  # String token
                    res = self._split_string_token(tok, remaining)
                    if res is None:
                        it = self._abort_segment(tok, it, row, pos, remaining, trailer_segment)
                        break
                    part1, part2 = res
                    pos = self._place_token(part1, row, pos)
                    it = self._abort_segment(part2, it, row, pos, 1, trailer_segment)
                
                else:
                    # Leave the remaining to the next segment
                    it = self._abort_segment(tok, it, row, pos, remaining, trailer_segment)
                break

        try:
            next(it)
        except StopIteration:
            min_lspaces = min(
                ([c == " " for c in row] + [False]).index(False)
            for row in grid
        )
            return '\n'.join(
                ''.join(row).rstrip()[min_lspaces:]
                for row in grid
                if not all(c == " " for c in row)
            )
        else:
            return None

    def _abort_segment(
        self,
        tok: Token,
        it: Iterator[Token],
        row: list[str],
        pos: int,
        remaining: int,
        trailer_segment: bool,
        add_backslash=False,
    ) -> Iterator[Token]:
        tok.value = tok.value.lstrip()  # The leading whitespace is not needed
        cpp_comment = trailer_segment and not add_backslash
        if add_backslash and remaining > 0:
            self._fill_with_comment(row, pos, remaining - 1, cpp_comment)
            row[pos + remaining - 1] = '\\'
        else:
            self._fill_with_comment(row, pos, remaining, cpp_comment)
        return chain([tok], it)  # Put the hash token back for the next row

    def _fill_with_comment(
        self, row: list[str], start: int, length: int, cpp_comment: bool,
    ):
        if row[start-1] == '/':
            row[start] = ' '
            start += 1
        if length >= 4:
            row[start:start+2] = '/*'
            for i in range(2, length-2):
                row[start+i] = next(self.comment_fill)
            row[start+length-2:start+length] = '*/'
        elif length >= 2 and cpp_comment:
            row[start:start+2] = '//'
            for i in range(2, length-2):
                row[start+i] = next(self.comment_fill)
        else:
            row[start:start+length] = ' ' * length

    def _place_token(self, tok: Token, row: list[str], pos: int) -> int:
        # Place the whole token
        row[pos:pos + len(tok.value)] = tok.value
        return pos + len(tok.value)

    def _split_string_token(
        self, tok: Token, remaining: int
    ) -> tuple[Token, Token] | None:
        # Ensure escape sequences work well, eg "\n" won't split into "\""n"
        backslashes = 0
        for ch in reversed(tok.value[:remaining - 1]):
            if ch != '\\':
                break
            backslashes += 1
        end_idx = remaining - (2 if backslashes % 2 != 0 else 1)
        if end_idx <= 0:
            return None
        part1 = tok.value[:end_idx] + '"'
        part2 = '"' + tok.value[end_idx:]
        return Token(part1, tok.preproc, tok.preproc_pos), Token(part2, tok.preproc, tok.preproc_pos)


class Pattern:
    def __init__(self, grid: list[list[int]]):
        self._grid = grid
        self.grid: list[list[int]] = grid[:][:]

    def scale(self, factor: float):
        rows = round(len(self._grid) * factor)
        cols = round(len(self._grid[0]) * factor)
        self.grid = []
        for i in range(rows):
            self.grid.append([])
            for j in range(cols):
                y = int(i / factor)
                x = int(j / factor)
                self.grid[-1].append(self._grid[y][x])

    def smooth(self, radius=1):
        # Pretty inefficient, but at least works for now
        grid = self.grid[:][:]
        rows = len(grid)
        cols = len(grid[0])
        smoothed = [[0 for j in range(cols)] for i in range(rows)]
        for _ in range(radius):
            grid.append([0 for i in range(cols)])
            grid.insert(0, [0 for i in range(cols)])
        for i in range(rows + 2*radius):
            for _ in range(radius):
                grid[i].append(0)
                grid[i].insert(0, 0)
        
        for i in range(rows):
            for j in range(cols):
                total = 0
                for r1 in range(-radius, radius+1):
                    for r2 in range(-radius, radius+1):
                        total += grid[i+r1][j+r2]
                smoothed[i][j] = total // (2*radius + 1)**2
        self.grid = smoothed

    def to_str(self, threshold=100) -> str:
        string = ""
        for row in self.grid:
            for col in row:
                if col >= threshold:
                    string += "##"
                else:
                    string += "  "
            string += "\n"
        return string


if __name__ == "__main__":
    import argparse
    argparser = argparse.ArgumentParser()
    argparser.add_argument('source', help='C source file')
    argparser.add_argument(
        '-i', dest='image', required=True, help='Your input image (required)'
    )
    argparser.add_argument(
        '-o', dest='output', default='out.c',
        help='Where to save the result (default out.c)'
    )
    argparser.add_argument(
        '-t', dest='threshold', type=int, default=100,
        help='Brightness threshold (0-255, lower = darker, default 100)'
    )
    argparser.add_argument(
        '-s', dest='scale', type=float,
        help='Manually scale the image'
    )
    argparser.add_argument(
        '-c', dest='comment_fill', default='_',
        help='Character(s) to fill comment areas with (defaults to be "_")'
    )
    argparser.add_argument(
        '-f', dest='comment_file',
        help='Use text from a file as comment filler'
    )
    args = argparser.parse_args()

    cfile = args.source
    imgfile = args.image
    output = args.output
    threshold = args.threshold
    scale = args.scale
    comment_fill = args.comment_fill
    comment_file = args.comment_file

    img = Image.open(imgfile).convert('L')
    cols, rows = img.size
    grid: list[list[int]] = []
    for y in range(rows):
        grid.append([])
        for x in range(cols):
            pixel = img.getpixel((x, y))
            grid[-1].append(int(pixel))  # type: ignore
    pattern = Pattern(grid)
    
    with open(cfile, encoding='utf-8') as f:
        source = f.read()
    if comment_file is not None:
        with open(comment_file, encoding='utf-8') as f:
            comment_fill = f.read()
    
    # Make sure there are no newlines & nested comments in the comment fill
    comment_fill = re.sub(r'\n', '', comment_fill)
    comment_fill = re.sub(r"/\*.*?\*/", " ", comment_fill, flags=re.DOTALL)
    comment_fill = re.sub(r"//.+", "", comment_fill)
    comment_fill = re.sub(r"\s+", "", comment_fill, flags=re.DOTALL)

    cshaper = CCodeShaper(source, comment_fill)
    print(f"Amount of tokens: {len(cshaper.toks)}")

    if scale is not None:
        pattern.scale(scale)
        result = cshaper.fit_pattern(pattern, threshold)
        if result is None:
            print("Scale too small")
            exit(1)
        print(result)
        exit(0)

    # A bit of crappy code that auto-resizes the image to fit the code in an optimal way
    factor = 1.0
    result = cshaper.fit_pattern(pattern, threshold)
    if result is not None:
        i = 1
        while True:
            i += 0.01
            factor = 1 / i
            pattern.scale(factor)
            new = cshaper.fit_pattern(pattern, threshold)
            if new is None:
                print(f"Scale: {factor:.2f}")
                break
            print(f"Scale: {factor:.2f}, too big")
            result = new
    else:
        while True:
            factor += 0.01
            pattern.scale(factor)
            pattern.smooth(int(factor / 2))
            result = cshaper.fit_pattern(pattern, threshold)

            if result is None:
                print(f"Scale: {factor:.2f}, too small")
            else:
                print(f"Scale: {factor:.2f}")
                break
    
    with open(output, 'w', encoding='utf-8') as f:
        f.write(result)
