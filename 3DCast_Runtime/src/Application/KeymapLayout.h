#pragma once

#include <string>
#include <3DCast/Input/MousebuttonCodes.h>
#include <3DCast/Input/KeyCodes.h>
#include <3DCast/Input/Input.h>
#include <unordered_map>
#include <list>

namespace Runtime::Application
{
    enum class KEY_ACTION
    {
        NONE = 0,
        CAMERA_FW,
        CAMERA_BW,
        CAMERA_L,
        CAMERA_R,
        CAMERA_UP,
        CAMERA_DOWN,
        CAMERA_SPEED_INC,
        CAMERA_SPEED_DEC,
        OBJ_ROT,
        OBJ_SCALE,
        OBJ_SNAP,
        ENT_DELETE,
        ENT_NEW,
        ENT_DUPLICATE,
        COMP_ADD,
        APP_EXIT,
        VIEWPORT_SWITCH,
        RENDERMODE_TOGGLE
    };

    class Keymap
    {
    public:
        inline static std::unordered_map<KEY_ACTION, std::list<int>> Layout{
            {KEY_ACTION::CAMERA_FW, {CAST_KEY_W}},
            {KEY_ACTION::CAMERA_BW, {CAST_KEY_S}},
            {KEY_ACTION::CAMERA_L, {CAST_KEY_A}},
            {KEY_ACTION::CAMERA_R, {CAST_KEY_D}},
            {KEY_ACTION::CAMERA_UP, {CAST_KEY_UP}},
            {KEY_ACTION::CAMERA_DOWN, {CAST_KEY_DOWN}},
            {KEY_ACTION::CAMERA_SPEED_INC, {CAST_MOUSE_UP}},
            {KEY_ACTION::CAMERA_SPEED_DEC, {CAST_MOUSE_DOWN}},
            {KEY_ACTION::OBJ_ROT, {CAST_KEY_E}},
            {KEY_ACTION::OBJ_SCALE, {CAST_KEY_R}},
            {KEY_ACTION::OBJ_SNAP, {CAST_KEY_LEFT_CONTROL}},
            {KEY_ACTION::ENT_DELETE, {CAST_KEY_DELETE}},
            {KEY_ACTION::ENT_NEW, {CAST_KEY_LEFT_CONTROL, CAST_KEY_N}},
            {KEY_ACTION::ENT_DUPLICATE, {CAST_KEY_LEFT_CONTROL, CAST_KEY_D}},
            {KEY_ACTION::COMP_ADD, {CAST_KEY_LEFT_CONTROL, CAST_KEY_A}},
            {KEY_ACTION::APP_EXIT, {CAST_KEY_LEFT_CONTROL, CAST_KEY_ESCAPE}},
            {KEY_ACTION::VIEWPORT_SWITCH, {CAST_KEY_LEFT_CONTROL, CAST_KEY_TAB}},
            {KEY_ACTION::RENDERMODE_TOGGLE, {CAST_KEY_LEFT_CONTROL, CAST_KEY_M}}
        };

        // bool states if mapping can be changed by the user
        inline static std::unordered_map<KEY_ACTION, std::tuple<std::string, bool>> ActionToString{
            {KEY_ACTION::CAMERA_FW, {"Move Camera Forward", true }},
            {KEY_ACTION::CAMERA_BW, {"Move Camera Backward", true }},
            {KEY_ACTION::CAMERA_L, {"Move Camera Left", true }},
            {KEY_ACTION::CAMERA_R, {"Move Camera Right", true }},
            {KEY_ACTION::CAMERA_UP, {"Move Camera Up", true }},
            {KEY_ACTION::CAMERA_DOWN, {"Move Camera Down", true }},
            {KEY_ACTION::CAMERA_SPEED_INC, {"Increase Camera Speed", false }},
            {KEY_ACTION::CAMERA_SPEED_DEC, {"Decrease Camera Speed", false }},
            {KEY_ACTION::OBJ_ROT, {"Adjust Object Rotate", true }},
            {KEY_ACTION::OBJ_SCALE, {"Adjust Object Scale", true }},
            {KEY_ACTION::OBJ_SNAP, {"Snap Object to Grid", true }},
            {KEY_ACTION::ENT_DELETE, {"Delete Entity", true }},
            {KEY_ACTION::ENT_NEW, {"Create New Entity", true }},
            {KEY_ACTION::ENT_DUPLICATE, {"Duplicate Entity", true }},
            {KEY_ACTION::COMP_ADD, {"Add Component", true }},
            {KEY_ACTION::APP_EXIT, {"Exit Application", false }},
            {KEY_ACTION::VIEWPORT_SWITCH, {"Switch Viewport", true }},
            {KEY_ACTION::RENDERMODE_TOGGLE, {"Toggle Render Mode", true }}
        };

        static inline bool IsActionActive(const KEY_ACTION action)
        {
            bool pressed = true;
            for (const auto& key : Layout[action])
            {
                pressed &= Cast::Input::IsKeyPressed(key);
            }

            return pressed;
        }

        inline static const char* KeyToString(const int key)
        {
            switch (key)
            {
                case CAST_KEY_SPACE: return "Space";
                case CAST_KEY_APOSTROPHE: return "'";
                case CAST_KEY_COMMA: return ",";
                case CAST_KEY_MINUS: return "-";
                case CAST_KEY_PERIOD: return ".";
                case CAST_KEY_SLASH: return "/";
                case CAST_KEY_0: return "0";
                case CAST_KEY_1: return "1";
                case CAST_KEY_2: return "2";
                case CAST_KEY_3: return "3";
                case CAST_KEY_4: return "4";
                case CAST_KEY_5: return "5";
                case CAST_KEY_6: return "6";
                case CAST_KEY_7: return "7";
                case CAST_KEY_8: return "8";
                case CAST_KEY_9: return "9";
                case CAST_KEY_SEMICOLON: return ";";
                case CAST_KEY_EQUAL: return "=";
                case CAST_KEY_A: return "A";
                case CAST_KEY_B: return "B";
                case CAST_KEY_C: return "C";
                case CAST_KEY_D: return "D";
                case CAST_KEY_E: return "E";
                case CAST_KEY_F: return "F";
                case CAST_KEY_G: return "G";
                case CAST_KEY_H: return "H";
                case CAST_KEY_I: return "I";
                case CAST_KEY_J: return "J";
                case CAST_KEY_K: return "K";
                case CAST_KEY_L: return "L";
                case CAST_KEY_M: return "M";
                case CAST_KEY_N: return "N";
                case CAST_KEY_O: return "O";
                case CAST_KEY_P: return "P";
                case CAST_KEY_Q: return "Q";
                case CAST_KEY_R: return "R";
                case CAST_KEY_S: return "S";
                case CAST_KEY_T: return "T";
                case CAST_KEY_U: return "U";
                case CAST_KEY_V: return "V";
                case CAST_KEY_W: return "W";
                case CAST_KEY_X: return "X";
                case CAST_KEY_Y: return "Y";
                case CAST_KEY_Z: return "Z";
                case CAST_KEY_LEFT_BRACKET: return "[";
                case CAST_KEY_BACKSLASH: return "\\";
                case CAST_KEY_RIGHT_BRACKET: return "]";
                case CAST_KEY_RIGHT: return "Right Arrow";
                case CAST_KEY_LEFT: return "Left Arrow";
                case CAST_KEY_UP: return "Up Arrow";
                case CAST_KEY_DOWN: return "Down Arrow";
                case CAST_KEY_F1: return "F1";
                case CAST_KEY_F2: return "F2";
                case CAST_KEY_F3: return "F3";
                case CAST_KEY_F4: return "F4";
                case CAST_KEY_F5: return "F5";
                case CAST_KEY_F6: return "F6";
                case CAST_KEY_F7: return "F7";
                case CAST_KEY_F8: return "F8";
                case CAST_KEY_F9: return "F9";
                case CAST_KEY_F10: return "F10";
                case CAST_KEY_F11: return "F11";
                case CAST_KEY_F12: return "F12";
                case CAST_KEY_KP_0: return "NP 0";
                case CAST_KEY_KP_1: return "NP 1";
                case CAST_KEY_KP_2: return "NP 2";
                case CAST_KEY_KP_3: return "NP 3";
                case CAST_KEY_KP_4: return "NP 4";
                case CAST_KEY_KP_5: return "NP 5";
                case CAST_KEY_KP_6: return "NP 6";
                case CAST_KEY_KP_7: return "NP 7";
                case CAST_KEY_KP_8: return "NP 8";
                case CAST_KEY_KP_9: return "NP 9";
                case CAST_KEY_ESCAPE: return "Escape";
                case CAST_KEY_ENTER: return "Enter";
                case CAST_KEY_TAB: return "Tab";
                case CAST_KEY_BACKSPACE: return "Backspace";
                case CAST_KEY_INSERT: return "Insert";
                case CAST_KEY_DELETE: return "Delete";
                case CAST_KEY_KP_DECIMAL: return "NP Decimal";
                case CAST_KEY_KP_DIVIDE: return "NP Divide";
                case CAST_KEY_KP_MULTIPLY: return "NP Multiply";
                case CAST_KEY_KP_SUBTRACT: return "NP Subtract";
                case CAST_KEY_KP_ADD: return "NP Add";
                case CAST_KEY_KP_ENTER: return "NP Enter";
                case CAST_KEY_KP_EQUAL: return "NP Equal";
                case CAST_KEY_LEFT_SHIFT: return "Left Shift";
                case CAST_KEY_LEFT_CONTROL: return "Left Control";
                case CAST_KEY_LEFT_ALT: return "Left Alt";
                case CAST_KEY_LEFT_SUPER: return "Left Super";
                case CAST_KEY_RIGHT_SHIFT: return "Right Shift";
                case CAST_KEY_RIGHT_CONTROL: return "Right Control";
                case CAST_KEY_RIGHT_ALT: return "Right Alt";
                case CAST_KEY_RIGHT_SUPER: return "Right Super";
                case CAST_MOUSE_UP: return "Scroll Up";
                case CAST_MOUSE_DOWN: return "Scroll Down";
                default: return std::to_string(key).c_str();
            }
        }

        constexpr static inline int AvailableKeys[] = {
            CAST_KEY_SPACE,
            CAST_KEY_APOSTROPHE,
            CAST_KEY_COMMA,
            CAST_KEY_MINUS,
            CAST_KEY_PERIOD,
            CAST_KEY_SLASH,
            CAST_KEY_0,
            CAST_KEY_1,
            CAST_KEY_2,
            CAST_KEY_3,
            CAST_KEY_4,
            CAST_KEY_5,
            CAST_KEY_6,
            CAST_KEY_7,
            CAST_KEY_8,
            CAST_KEY_9,
            CAST_KEY_SEMICOLON,
            CAST_KEY_EQUAL,
            CAST_KEY_A,
            CAST_KEY_B,
            CAST_KEY_C,
            CAST_KEY_D,
            CAST_KEY_E,
            CAST_KEY_F,
            CAST_KEY_G,
            CAST_KEY_H,
            CAST_KEY_I,
            CAST_KEY_J,
            CAST_KEY_K,
            CAST_KEY_L,
            CAST_KEY_M,
            CAST_KEY_N,
            CAST_KEY_O,
            CAST_KEY_P,
            CAST_KEY_Q,
            CAST_KEY_R,
            CAST_KEY_S,
            CAST_KEY_T,
            CAST_KEY_U,
            CAST_KEY_V,
            CAST_KEY_W,
            CAST_KEY_X,
            CAST_KEY_Y,
            CAST_KEY_Z,
            CAST_KEY_LEFT_BRACKET,
            CAST_KEY_BACKSLASH,
            CAST_KEY_RIGHT_BRACKET,
            CAST_KEY_RIGHT,
            CAST_KEY_LEFT,
            CAST_KEY_UP,
            CAST_KEY_DOWN,
            CAST_KEY_F1,
            CAST_KEY_F2,
            CAST_KEY_F3,
            CAST_KEY_F4,
            CAST_KEY_F5,
            CAST_KEY_F6,
            CAST_KEY_F7,
            CAST_KEY_F8,
            CAST_KEY_F9,
            CAST_KEY_F10,
            CAST_KEY_F11,
            CAST_KEY_F12,
            CAST_KEY_KP_0,
            CAST_KEY_KP_1,
            CAST_KEY_KP_2,
            CAST_KEY_KP_3,
            CAST_KEY_KP_4,
            CAST_KEY_KP_5,
            CAST_KEY_KP_6,
            CAST_KEY_KP_7,
            CAST_KEY_KP_8,
            CAST_KEY_KP_9,
            CAST_KEY_KP_DECIMAL,
            CAST_KEY_KP_DIVIDE,
            CAST_KEY_KP_MULTIPLY,
            CAST_KEY_KP_SUBTRACT,
            CAST_KEY_KP_ADD,
            CAST_KEY_KP_ENTER,
            CAST_KEY_KP_EQUAL,
            CAST_KEY_LEFT_SHIFT,
            CAST_KEY_LEFT_CONTROL,
            CAST_KEY_LEFT_ALT,
            CAST_KEY_LEFT_SUPER,
            CAST_KEY_RIGHT_SHIFT,
            CAST_KEY_RIGHT_CONTROL,
            CAST_KEY_RIGHT_ALT,
            CAST_KEY_RIGHT_SUPER,
        };
    };
}
