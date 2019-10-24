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
        self.text_height = 30
        self.font = pygame.font.SysFont("Calibri", 16)
        # calculated params
        self.window_width = self.router_count[0] * (self.router_width + self.space_width) - self.space_width
        self.window_height = self.router_count[1] * (
                self.router_block_height * self.block_per_router + self.space_height
        ) - self.space_height + self.text_height

        self.game_log = game_log
        pygame.display.set_caption('Routers')
        self.screen_res = [self.window_width , self.window_height]
        self.screen = pygame.display.set_mode(self.screen_res, pygame.HWSURFACE, 32)

    def event_loop(self):
        for event in pygame.event.get():
            if event.type == QUIT:
                pygame.quit()
                sys.exit()
            if event.type == KEYDOWN:
                if event.key == K_LEFT:
                    pass
                if event.key == K_RIGHT:
                    pass

    def draw(self):
        pass

    def run(self):
        while True:
            self.event_loop()
            self.draw()
            pygame.display.update()
