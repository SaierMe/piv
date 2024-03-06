#ifndef _PIV_IMGUI_HPP
#define _PIV_IMGUI_HPP

#include "../src/piv_string.hpp"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#ifndef STBI_WINDOWS_UTF8
#define STBI_WINDOWS_UTF8
#endif
#ifndef STB_IMAGE_STATIC
#define STB_IMAGE_STATIC
#endif
#include "../src/stb_image.h"

#ifndef IM_ARRAY_AND_SIZE
#define IM_ARRAY_AND_SIZE(_ARR) (_ARR), ((int)(sizeof(_ARR) / sizeof(*(_ARR))))
#endif
#ifndef IM_STRARRAY_PTR_AND_SIZE
#define IM_STRARRAY_PTR_AND_SIZE(_ARR) (_ARR), ((int)((_ARR)->size()))
#endif

namespace ImGui
{
    static void TextTooltip(const char *text, const char *desc)
    {
        ImGui::TextDisabled(text);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    static void FontSelector(const char *label)
    {
        ImGuiIO &io = ImGui::GetIO();
        ImFont *font_current = ImGui::GetFont();
        if (ImGui::BeginCombo(label, font_current->GetDebugName()))
        {
            for (int n = 0; n < io.Fonts->Fonts.Size; n++)
            {
                ImFont *font = io.Fonts->Fonts[n];
                ImGui::PushID((void *)font);
                if (ImGui::Selectable(font->GetDebugName(), font == font_current))
                    io.FontDefault = font;
                ImGui::PopID();
            }
            ImGui::EndCombo();
        }
    }

    template <typename T>
    bool DragScalarT(const char *label, ImGuiDataType data_type, T *p_v, float v_speed, T v_min, T v_max, const char *format, ImGuiSliderFlags flags)
    {
        return ImGui::DragScalar(label, data_type, p_v, v_speed, &v_min, &v_max, format, flags);
    }

    template <typename T>
    bool DragScalarTN(const char *label, ImGuiDataType data_type, T *arr, int components, float v_speed, T v_min, T v_max, const char *format, ImGuiSliderFlags flags)
    {
        return ImGui::DragScalarN(label, data_type, arr, components, v_speed, &v_min, &v_max, format, flags);
    }

    template <typename T>
    bool SliderScalarT(const char *label, ImGuiDataType data_type, T *p_v, T v_min, T v_max, const char *format, ImGuiSliderFlags flags)
    {
        return ImGui::SliderScalar(label, data_type, p_v, &v_min, &v_max, format, flags);
    }

    template <typename T>
    bool SliderScalarTN(const char *label, ImGuiDataType data_type, T *arr, int components, T v_min, T v_max, const char *format, ImGuiSliderFlags flags)
    {
        return ImGui::SliderScalarN(label, data_type, arr, components, &v_min, &v_max, format, flags);
    }

    template <typename T>
    bool VSliderScalarT(const char *label, const ImVec2 &size, ImGuiDataType data_type, T *p_v, T v_min, T v_max, const char *format, ImGuiSliderFlags flags)
    {
        return ImGui::VSliderScalar(label, size, data_type, p_v, &v_min, &v_max, format, flags);
    }

    template <typename T>
    bool InputScalarT(const char *label, ImGuiDataType data_type, T *p_v, T step, T step_fast, const char *format, ImGuiInputTextFlags flags)
    {
        if (data_type == ImGuiDataType_Float || data_type == ImGuiDataType_Double)
            flags |= ImGuiInputTextFlags_CharsScientific;
        return ImGui::InputScalar(label, data_type, p_v, (void *)(step > 0 ? &step : NULL), (void *)(step_fast > 0 ? &step_fast : NULL), format, flags);
    }

    template <typename T>
    bool InputScalarTN(const char *label, ImGuiDataType data_type, T *arr, int components, T step, T step_fast, const char *format, ImGuiInputTextFlags flags)
    {
        if (data_type == ImGuiDataType_Float || data_type == ImGuiDataType_Double)
            flags |= ImGuiInputTextFlags_CharsScientific;
        return ImGui::InputScalarN(label, data_type, arr, components, (void *)(step > 0 ? &step : NULL), (void *)(step_fast > 0 ? &step_fast : NULL), format, flags);
    }

} // namespace ImGui

#endif // _PIV_IMGUI_HPP
