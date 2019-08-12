#pragma once

#include <cstdint>
#include <array>
#include <map>
#include <functional>
#include <utility>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window.hpp"
#include "utils.hpp"

enum class EventType: uint8_t {
    KeyPressed, KeyHeld, KeyReleased,
    MouseButtonPressed, MouseButtonHeld, MouseButtonReleased,
    MouseMoved,
    MouseScrolled,
    WindowResized, WindowMoved, WindowFocused, WindowDefocused, WindowClosed,
    Max,
};

struct Event { };

struct KeyEvent: public Event {
    inline KeyEvent(int key, int mods): key(key), mods(mods) { }

    inline int get_key()  const { return this->key; }
    inline int get_mods() const { return this->mods; }

    inline bool has_mod(int mod) const { return get_mods() & mod; }

    protected:
        int key, mods;
};

struct KeyPressedEvent: public KeyEvent {
    inline KeyPressedEvent(int key, int mods): KeyEvent(key, mods) { }

    static inline constexpr EventType get_type() { return EventType::KeyPressed; };
};

struct KeyHeldEvent: public KeyEvent {
    inline KeyHeldEvent(int key, int mods): KeyEvent(key, mods) {
        ++this->key_cnts[key];
    }

    inline int        get_repeats() const  { return key_cnts[get_key()]; }
    static inline int get_repeats(int key) { return key_cnts[key]; }

    static inline void reset_key(int key) { key_cnts[key] = 0; }

    static inline constexpr EventType get_type() { return EventType::KeyHeld; };

    protected:
        static inline std::array<std::uint32_t, GLFW_KEY_LAST> key_cnts;
};

struct KeyReleasedEvent: public KeyEvent {
    inline KeyReleasedEvent(int key, int mods): KeyEvent(key, mods) {
        KeyHeldEvent::reset_key(key);
    }

    static inline constexpr EventType get_type() { return EventType::KeyReleased; };
};

struct MouseEvent: public Event, Position<float> {
    inline MouseEvent(float x, float y): Position(x, y) { }
};

struct MouseMovedEvent: public MouseEvent {
    inline MouseMovedEvent(float x, float y): MouseEvent(x, y) { }

    static inline constexpr EventType get_type() { return EventType::MouseMoved; };
};

struct MouseScrolledEvent: public MouseEvent {
    inline MouseScrolledEvent(float x, float y): MouseEvent(x, y) { } // Cheat, x and y aren't actually mouse coordinates

    static inline constexpr EventType get_type() { return EventType::MouseScrolled; };
};

struct MouseButtonPressedEvent: public KeyEvent {
    inline MouseButtonPressedEvent(int key, int mods): KeyEvent(key, mods) { }

    static inline constexpr EventType get_type() { return EventType::MouseButtonPressed; };
};

struct MouseButtonHeldEvent: public KeyEvent {
    inline MouseButtonHeldEvent(int key, int mods): KeyEvent(key, mods) {
        ++this->key_cnts[key];
    }

    inline int        get_repeats() const  { return key_cnts[get_key()]; }
    static inline int get_repeats(int key) { return key_cnts[key]; }

    static inline void reset_key(int key) { key_cnts[key] = 0; }

    static inline constexpr EventType get_type() { return EventType::MouseButtonHeld; };

    protected:
        static inline std::array<std::uint32_t, GLFW_MOUSE_BUTTON_LAST> key_cnts;
};

struct MouseButtonReleasedEvent: public KeyEvent {
    inline MouseButtonReleasedEvent(int key, int mods): KeyEvent(key, mods) {
        MouseButtonHeldEvent::reset_key(key);
    }

    static inline constexpr EventType get_type() { return EventType::MouseButtonReleased; };
};

struct WindowResizedEvent: public Event, Area<int> {
    inline WindowResizedEvent(int w, int h): Area(w, h) { }

    static inline constexpr EventType get_type() { return EventType::WindowResized; }
};

struct WindowMovedEvent: public Event, Position<int> {
    inline WindowMovedEvent(int x, int y): Position(x, y) { }

    static inline constexpr EventType get_type() { return EventType::WindowMoved; }
};

struct WindowFocusedEvent: public Event {
    inline WindowFocusedEvent() = default;

    static inline constexpr EventType get_type() { return EventType::WindowFocused; }
};

struct WindowDefocusedEvent: public Event {
    WindowDefocusedEvent() = default;

    static inline constexpr EventType get_type() { return EventType::WindowDefocused; }
};

struct WindowClosedEvent: public Event {
    inline WindowClosedEvent() = default;

    static inline constexpr EventType get_type() { return EventType::WindowClosed; }
};

class InputManager {
    public:
        template <typename T = Event>
        using Callback = std::function<void(T &)>;

        InputManager(Window *window): window(window) {
            s_this = this;
            set_window_callbacks();
        }

        template <typename T>
        std::size_t register_callback(Callback<T> cb) {
            this->callbacks[(int)T::get_type()].insert({this->cur_handle, *(Callback<> *)&cb});
            return this->cur_handle++;
        }

        template <typename T>
        void remove_callback(std::size_t handle) {
            this->callbacks[(int)T::get_type()].erase(handle);
        }

        template <typename T>
        void process(T event) const {
            for (auto &[id, cb]: this->callbacks[(int)event.get_type()])
                cb(event);
        }

        void set_window_callbacks() const {
            this->window->set_keys_cb(keys_cb);
            this->window->set_cursor_cb(cursor_cb);
            this->window->set_scroll_cb(scroll_cb);
            this->window->set_click_cb(click_cb);
            this->window->set_pos_cb(window_pos_cb);
            this->window->set_size_cb(window_size_cb);
            this->window->set_focus_cb(window_focus_cb);
            this->window->set_close_cb(window_close_cb);
        }

    private:
        static void keys_cb(GLFWwindow *win, int key, int scancode, int action, int modifiers) {
            if (action == GLFW_PRESS)
                s_this->process(KeyPressedEvent(key, modifiers));
            else if (action == GLFW_RELEASE)
                s_this->process(KeyReleasedEvent(key, modifiers));
            else
                s_this->process(KeyHeldEvent(key, modifiers));
        }

        static void cursor_cb(GLFWwindow *window, double x, double y) {
            s_this->process(MouseMovedEvent(x, y));
        }

        static void scroll_cb(GLFWwindow *window, double x, double y) {
            s_this->process(MouseScrolledEvent(x, y));
        }

        static void click_cb(GLFWwindow *window, int key, int action, int modifiers) {
            if (action == GLFW_PRESS)
                s_this->process(MouseButtonPressedEvent(key, modifiers));
            else if (action == GLFW_RELEASE)
                s_this->process(MouseButtonReleasedEvent(key, modifiers));
            else
                s_this->process(MouseButtonHeldEvent(key, modifiers));
        }

        static void window_pos_cb(GLFWwindow* window, int x, int y) {
            s_this->process(WindowMovedEvent(x, y));
        }

        static void window_size_cb(GLFWwindow* window, int width, int height) {
            s_this->process(WindowResizedEvent(width, height));
        }

        static void window_focus_cb(GLFWwindow* window, int focused) {
            if (focused)
                s_this->process(WindowFocusedEvent());
            else
                s_this->process(WindowDefocusedEvent());
        }

        static void window_close_cb(GLFWwindow* window) {
            s_this->process(WindowClosedEvent());
        }

    protected:
        static inline InputManager *s_this;
        Window *window;
        std::size_t cur_handle = 0;
        std::array<std::map<std::size_t, Callback<>>, (std::size_t)EventType::Max> callbacks;
};
