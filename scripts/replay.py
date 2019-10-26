#!/usr/bin/python3

import argparse
import router_game
import display


def parse_args():
    parser = argparse.ArgumentParser(description='Game replay')
    parser.add_argument('-l', '--log', type=str, required=True,
                        help='game log downloaded form the server')
    return parser.parse_args()


def main():
    args = parse_args()
    game_log = router_game.load_log(args.log)
    window = display.DisplayWindow(game_log)
    window.run()


if __name__ == "__main__":
    main()
