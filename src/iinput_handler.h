#pragma once

class IInputHandler
{
public:
    virtual ~IInputHandler() = default;
    virtual auto handleClick(double xpos, double ypos, bool isDragging,
            bool isDragStart) -> void = 0;
    virtual auto handleDoubleClick(double xpos, double ypos) -> void = 0;
    virtual auto handleNewGame() -> void = 0;
    virtual auto handleRestart() -> void {}
    virtual auto handlePause() -> void {}
    virtual auto handleUndo() -> void {}
    virtual auto handleRedo() -> void {}
    virtual auto handlePrintCards() -> void {}
};
