import re


class Router:
    def __init__(self, id, stat):
        self.id = id
        self.stat = stat


class GameFrame:
    def __init__(self):
        self.command = None
        self.message = None
        self.request_id = None
        self.prev_error = None
        self.routers = []


class GameLog:
    def __init__(self, game_frames):
        self.game_frames = game_frames


def read_line(inp):
    res = inp.readline()
    if res == "":
        return None
    return res.strip()


def re_timestamp(txt):
    return re.compile(r"\d{4}-\d{2}-\d{2}T\d{2}[:]\d{2}[:]\d{2}.\d+Z\s+"+txt)


def is_read_start(txt, pattern=re_timestamp("Started to read")):
    return pattern.match(txt)


def is_frame_end(txt, pattern=re.compile(r'[.]')):
    return pattern.match(txt)


def is_my_command(txt, pattern=re_timestamp("Readed:(.*)$")):
    return pattern.match(txt)


def is_request_id(txt, pattern=re_timestamp(r"Start write: REQUEST(.*)$")):
    return pattern.match(txt)


def is_writen_ended(txt, pattern=re_timestamp(r"Write ended")):
    return pattern.match(txt)


def is_router(txt, pattern=re.compile(r"ROUTER (\d+)\s+(\d+)")):
    return pattern.match(txt)


def is_prev_error(txt, pattern=re.compile(r"PREVIOUS (.*)$")):
    return pattern.match(txt)


def read_next(inp):
    next_frame = GameFrame()
    while True:
        line = read_line(inp)
        if line is None:
            return None
        elif is_frame_end(line):
            return next_frame
        elif is_my_command(line):
            next_frame.command = is_my_command(line).group(1)
        elif is_request_id(line):
            next_frame.request_id = is_request_id(line).group(1)
        elif is_router(line):
            res = is_router(line)
            next_frame.routers.append(Router(int(res.group(1)), res.group(2)))
        elif is_prev_error(line):
            next_frame.prev_error = is_prev_error(line).group(1)
        elif is_writen_ended(line) or is_read_start(line):
            pass
        else:
            print("Cant parse line: {}".format(line))


def load_log(file_path: str) -> GameLog:
    game_frames = []
    with open(file_path) as inp:
        while True:
            frame = read_next(inp)
            if frame is None:
                break
            game_frames.append(frame)
    return GameLog(game_frames)
