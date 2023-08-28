#pragma once

#include <string>

namespace RenderD7 {
    struct Message {
        Message(std::string t, std::string m) {
            title = t;
            message = m;
            animationframe = 0;
        }

        std::string title;
        std::string message;
        int animationframe;
    };

    void ProcessMessages();
    void PushMessage(const Message& msg);
    // Config
    void SetIdleStartFrame(int frame);
    void SetTotalAnimationFrames(int total_frames);
    void SetFadeOutStartFrame(int frame);
}