/*
** EPITECH PROJECT, 2026
** UiText.cpp
** File description:
** Shared UI font (DejaVu, Latin-1 + a few symbols) and text helpers
*/

#include "gui/UiText.hpp"
#include <sstream>

namespace {
    // Rendu en haute résolution puis réduit (filtre bilinéaire) : net aussi
    // bien en 14 px dans un panneau qu'en 90 px sur un titre de vidéo.
    constexpr int BASE_SIZE = 64;

    Font g_regular = {};
    Font g_bold    = {};
    bool g_loaded  = false;

    std::vector<int> codepoints()
    {
        std::vector<int> cps;
        for (int c = 32; c < 127; ++c)
            cps.push_back(c);
        for (int c = 160; c < 256; ++c)
            cps.push_back(c);
        // œ Œ ’ “ ” … – — • · → ← ↑ ↓ ★ ☆ ✓ ✗ ▶ ● ◆ ⚡
        const int extra[] = {0x153, 0x152, 0x2019, 0x201C, 0x201D, 0x2026,
                             0x2013, 0x2014, 0x2022, 0x2192, 0x2190, 0x2191,
                             0x2193, 0x2605, 0x2606, 0x2713, 0x2717, 0x25B6,
                             0x25CF, 0x25C6, 0x26A1};
        for (int c : extra)
            cps.push_back(c);
        return cps;
    }

    Font loadOne(const char *path)
    {
        if (!FileExists(path))
            return GetFontDefault();
        std::vector<int> cps = codepoints();
        Font f = LoadFontEx(path, BASE_SIZE, cps.data(),
                            static_cast<int>(cps.size()));
        if (f.texture.id == 0)
            return GetFontDefault();
        SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
        return f;
    }

    const Font &font(bool bold)
    {
        if (!g_loaded)
            UiText::load();
        return bold ? g_bold : g_regular;
    }

    float spacing(float size) { return size / 20.0f; }
}

void UiText::load()
{
    if (g_loaded)
        return;
    g_regular = loadOne("assets/fonts/DejaVuSans.ttf");
    g_bold    = loadOne("assets/fonts/DejaVuSans-Bold.ttf");
    g_loaded  = true;
}

void UiText::unload()
{
    if (!g_loaded)
        return;
    Font def = GetFontDefault();
    if (g_regular.texture.id != def.texture.id)
        UnloadFont(g_regular);
    if (g_bold.texture.id != def.texture.id)
        UnloadFont(g_bold);
    g_loaded = false;
}

void UiText::draw(const std::string &text, float x, float y, float size,
                  Color color, bool bold)
{
    DrawTextEx(font(bold), text.c_str(), {x, y}, size, spacing(size), color);
}

void UiText::drawShadow(const std::string &text, float x, float y,
                        float size, Color color, bool bold)
{
    float off = size > 30 ? 3.0f : 1.5f;
    Color shadow = {0, 0, 0, static_cast<unsigned char>(color.a * 0.7f)};
    draw(text, x + off, y + off, size, shadow, bold);
    draw(text, x, y, size, color, bold);
}

float UiText::width(const std::string &text, float size, bool bold)
{
    return MeasureTextEx(font(bold), text.c_str(), size, spacing(size)).x;
}

std::vector<std::string> UiText::wrap(const std::string &text, float size,
                                      float maxWidth, bool bold)
{
    std::vector<std::string> lines;
    std::istringstream paragraphs(text);
    std::string para;
    while (std::getline(paragraphs, para)) {
        std::istringstream words(para);
        std::string word, line;
        while (words >> word) {
            std::string candidate = line.empty() ? word : line + " " + word;
            if (!line.empty() && width(candidate, size, bold) > maxWidth) {
                lines.push_back(line);
                line = word;
            } else {
                line = candidate;
            }
        }
        lines.push_back(line);
    }
    return lines;
}

float UiText::drawWrapped(const std::string &text, float x, float y,
                          float size, float maxWidth, Color color,
                          float lineGap, bool bold)
{
    float lineH = size * lineGap;
    float cy = y;
    for (const std::string &line : wrap(text, size, maxWidth, bold)) {
        draw(line, x, cy, size, color, bold);
        cy += lineH;
    }
    return cy - y;
}
