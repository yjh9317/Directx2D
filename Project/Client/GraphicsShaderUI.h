#pragma once
#include "ResInfoUI.h"
class GraphicsShaderUI :
    public ResInfoUI
{
private:

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    GraphicsShaderUI();
    ~GraphicsShaderUI();
};