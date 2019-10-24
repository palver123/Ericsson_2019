from router_game import GameLog
import pygame
from pygame.locals import *
import sys

class Router:
    pass

class DisplayWindow:
    def __init__(self, game_log: GameLog):
        pygame.font.init()
        # modifiable configurations
        self.router_count = (7, 2)
        self.router_width = 150
        self.router_block_height = 30
        self.block_per_router = 10
        self.space_width = 20
        self.space_height = 20
        self.text_height = 50
        self.font = pygame.font.SysFont("Calibri", 32)
        # calculated params
        self.window_width = self.router_count[0] * (self.router_width + self.space_width) - self.space_width
        self.window_height = self.router_count[1] * (
                self.router_block_height * self.block_per_router + self.space_height
        ) - self.space_height + self.text_height

        self.game_log = game_log
        pygame.display.set_caption('Routers')
        self.screen_res = [self.window_width , self.window_height]
        self.screen = pygame.display.set_mode(self.screen_res, pygame.HWSURFACE, 32)
        self.act_index = 0

    def event_loop(self):
        for event in pygame.event.get():
            if event.type == QUIT:
                pygame.quit()
                sys.exit()
            if event.type == KEYDOWN:
                if event.key == K_LEFT:
                    self.act_index = max(0, self.act_index - 1)
                if event.key == K_RIGHT:
                    self.act_index = min(len(self.game_log.game_frames) - 1, self.act_index + 1)

    def draw_router(self, router, lt_coord):
        for i in range(0, self.block_per_router):
            left = lt_coord[0]
            top = lt_coord[1] + self.router_block_height * i
            color = (255, 127, 127) if router.stat[i] == "0" else (127, 255, 127)
            pygame.draw.rect(self.screen, color, (left, top, self.router_width, self.router_block_height))

    def draw_error(self, error_txt):
        top = self.router_count[1] * (self.router_block_height * self.block_per_router + self.space_height)
        left = 0
        red = (255, 0, 0)
        text = self.font.render(error_txt, True, red)
        self.screen.blit(text, (left, top))

    def draw(self):
        white = (255, 255, 255)
        self.screen.fill(white)
        frame = self.game_log.game_frames[self.act_index]
        if len(frame.routers) != self.router_count[0] * self.router_count[1]:
            raise Exception("Invalid number of routers: {}".format(len(frame.routers)))
        for i in range (0, len(frame.routers)):
            ys = i // self.router_count[0]
            xs = i % self.router_count[0]
            top = ys * (self.router_block_height * self.block_per_router + self.space_height)
            left = xs * (self.router_width + self.space_width)
            self.draw_router(frame.routers[i], (left, top))
        self.draw_error(frame.prev_error)
        pygame.display.update()

    def run(self):
        while True:
            self.event_loop()
            self.draw()
            pygame.display.update()
