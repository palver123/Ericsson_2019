import re

allReceivedMessagesPieces = []

class Router:
    def __init__(self, id, is_open):
        self.id = id
        self.is_open = is_open


class Data:
    def __init__(self, currRouter, currStore, messageId, fromRouter, toRouter, clockwise, pri):
        self.currRouter = currRouter
        self.currStore = currStore
        self.messageId = messageId
        self.fromRouter = fromRouter
        self.toRouter = toRouter
        self.clockwise = clockwise
        self.pri = pri


class GameFrame:
    def __init__(self):
        self.command = None
        self.message = None
        self.prev_error = ""
        self.routers = []
        self.dataPackages = []
        self.receivedMessagePieces = []


class GameLog:
    def __init__(self, game_frames):
        self.game_frames = game_frames

class TickHeader:
    def __init__(self, gameId, tickId, startRouterId):
        self.gameId = gameId
        self.tickId = tickId
        self.startRouterId = startRouterId

def read_line(inp):
    res = inp.readline()
    if res == "":
        return None
    return res.strip()


def re_timestamp(txt):
    return re.compile(r"\d{4}-\d{2}-\d{2}T\d{2}[:]\d{2}[:]\d{2}.\d+Z\s+"+txt)


def starts_with_timestamp(txt):
    return re.compile(r"^\d{4}-\d{2}-\d{2}T\d{2}[:]\d{2}[:]\d{2}.\d+Z.*").match(txt) != None


def is_frame_end(txt, pattern=re.compile(r'[.]')):
    return pattern.match(txt)


def is_my_command(txt, pattern=re_timestamp("Readed: (\d+)\s+(\d+)\s+(\d+)\s+(.*)$")):
    return pattern.match(txt)


def is_router(txt, pattern=re.compile(r"ROUTER (\d+)\s+(\d+)")):
    return pattern.match(txt)


def is_data(txt, pattern=re.compile(r"DATA (\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+[lr](\s+PRI)?$")):
    return pattern.match(txt)


def is_message_piece(txt, pattern=re.compile(r"MESSAGE (.+)$")):
    return pattern.match(txt)


def is_tick_header(txt, pattern=re.compile(r"REQUEST (\d+)\s+(\d+)\s+(\d+)$")):
    return pattern.match(txt)


def is_prev_error(txt, pattern=re.compile(r"PREVIOUS (.*)$")):
    return pattern.match(txt)


def read_next(inp):
    next_frame = GameFrame()
    for i in range (0, len(allReceivedMessagesPieces)):
        next_frame.receivedMessagePieces.append(allReceivedMessagesPieces[i])

    while True:
        line = read_line(inp)
        if line is None:
            return None
        elif is_frame_end(line):
            return next_frame
        elif is_my_command(line):
            next_frame.command = is_my_command(line).group(4)
        elif is_router(line):
            res = is_router(line)
            next_frame.routers.append(Router(int(res.group(1)), res.group(2)))
        elif is_data(line):
            res = is_data(line)
            next_frame.dataPackages.append(Data(int(res.group(1)), int(res.group(2)), res.group(4), int(res.group(5)), int(res.group(6)), line.endswith('r'), res.group(7) == " PRI"))
        elif is_message_piece(line):
            res = is_message_piece(line)
            msgPiece = res.group(1)
            allReceivedMessagesPieces.append(msgPiece)
            next_frame.receivedMessagePieces.append(msgPiece)
        elif is_prev_error(line):
            next_frame.prev_error = next_frame.prev_error + is_prev_error(line).group(1)
        elif starts_with_timestamp(line) or is_tick_header(line):
            pass
        else:
            print("Cant parse line: {}".format(line))
    next_frame.receivedMessagePieces.sort() # TODO This does a lexical sort which means: '10' < '1'


def load_log(file_path: str) -> GameLog:
    game_frames = []
    with open(file_path) as inp:
        while True:
            frame = read_next(inp)
            if frame is None or len(frame.routers) < 1:
                break
            game_frames.append(frame)
    return GameLog(game_frames)
