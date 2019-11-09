#pragma once

// Data packets can move from one router to another in 2 directions:
enum class HorizontalDirection : char {
    // Data packets move in a clockwise order if sent from router 0-6 and counter-clockwise if sent from 7-13
    LEFT = 'l',

    // Data packets move in a counter-clockwise order if sent from router 0-6 and clockwise if sent from 7-13
    RIGHT = 'r'
};

// Router rules can be modified in two directions.
enum class VerticalDirection : char {
    // All slots in the router inherit the state of the 'previous' slot (0th slot inherits the state of the highest index slot). Data packets inside the slots move in this direction too!
    NEGATIVE = '^',

    // All slots in the router inherit the state of the 'next' slot (the highest index slot inherits the state of the 0th slot). Data packets inside the slots move in this direction too!
    POSITIVE = 'v'
};

namespace Dir {
    inline VerticalDirection opposite(const VerticalDirection& d) {
        if (d == VerticalDirection::NEGATIVE) return VerticalDirection::POSITIVE;
        return VerticalDirection::NEGATIVE;
    }
    inline HorizontalDirection opposite(const HorizontalDirection& d) {
        if (d == HorizontalDirection::LEFT) return HorizontalDirection::RIGHT;
        return HorizontalDirection::LEFT;
    }

    inline int dirToint(const VerticalDirection& d) {
        if (d == VerticalDirection::NEGATIVE) return -1;
        return 1;
    }

    inline int dirToint(const HorizontalDirection& d) {
        if (d == HorizontalDirection::LEFT) return -1;
        return 1;
    }

}
