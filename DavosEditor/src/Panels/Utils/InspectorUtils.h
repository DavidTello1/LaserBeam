#pragma once
#include <Davos.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <ImGui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace Davos::InspectorUtils {

    static ImGuiID s_SelectedItem = 0;

	// --------------------------------------------------
    static bool _RenderField(const ImGuiID id, const char* label, bool isSelected, bool isReset, bool* out_reset)
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        if (window->SkipItems)
            return false;

        const float height = ImGui::GetFrameHeight();
        const float fullWidth = ImGui::GetContentRegionAvail().x;
        const float halfWidth = fullWidth * 0.5f;
        const ImVec2 padding = g.Style.FramePadding;

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size = { halfWidth, height };
        ImRect bb(pos, pos + size);

        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, id, nullptr, ImGuiItemFlags_NoNav))
            return false;

        const ImRect labelRect(ImVec2(pos.x + padding.x, pos.y + (height - g.FontSize) * 0.5f), ImVec2(pos.x + halfWidth - height, pos.y + height));
        const ImRect resetRect(ImVec2(labelRect.Max.x, pos.y), ImVec2(pos.x + halfWidth, pos.y + height));

        bool hovered, held;
        bool isPressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, ImGuiButtonFlags_AllowOverlap);

        bool resetHovered = false;
        bool resetHeld = false;
        if (isReset)
        {
            std::string resetLabelID = std::to_string(id) + "##reset";
            ImGuiID resetID = window->GetID(resetLabelID.c_str());
            *out_reset = ImGui::ButtonBehavior(resetRect, resetID, &resetHovered, &resetHeld, ImGuiButtonFlags_PressedOnClick);
        }

        if (isSelected)
        {
            const ImU32 selectedColor = ImGui::GetColorU32(ImGuiCol_FrameBgActive);
            window->DrawList->AddRectFilled(bb.Min, bb.Max, selectedColor);
        }

        if (label)
        {
            const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
            ImGui::RenderTextEllipsis(window->DrawList, labelRect.Min, labelRect.Max, labelRect.Max.x, label, nullptr, &label_size);
        }

        if (isReset)
        {
            const ImU32 iconColor = resetHovered ? IM_COL32(255, 255, 255, 255) : IM_COL32(230, 230, 230, 255);

            const float radius = height * 0.25f;
            const ImVec2 center = (resetRect.Min + resetRect.Max) * 0.5f;
            const float thickness = 1.5f;

            const int segments = 20;
            const float a_min = -IM_PI * 0.45f;
            const float a_max = IM_PI * 1.3f;
            window->DrawList->PathArcTo(center, radius, a_min, a_max, segments);
            window->DrawList->PathStroke(iconColor, false, thickness);

            const ImVec2 arcEnd = ImVec2(center.x + cosf(a_min) * radius, center.y + sinf(a_min) * radius);
            const ImVec2 arrowLength = ImVec2(radius * 0.9f, radius * 0.75f);
            window->DrawList->AddLine(arcEnd, ImVec2(arcEnd.x + arrowLength.x, arcEnd.y), iconColor, thickness);
            window->DrawList->AddLine(arcEnd, ImVec2(arcEnd.x, arcEnd.y + arrowLength.y), iconColor, thickness);
        }

        return isPressed || *out_reset;
    }

    static bool _RenderCheckbox(const char* label, bool* value)
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        if (window->SkipItems)
            return false;

        const ImGuiID id = window->GetID(label);

        const float height = ImGui::GetFrameHeight();
        const float width = ImGui::GetContentRegionAvail().x;
        const ImVec2 padding = g.Style.FramePadding;
        const float rounding = g.Style.FrameRounding;

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size = { width, height };
        ImRect bb(pos, pos + size);

        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, id))
            return false;

        const ImRect widgetRect(pos, ImVec2(pos.x + width, pos.y + height));

        const float checkboxSize = height * 0.75f;
        const ImRect checkRect(
            ImVec2(pos.x + padding.x * 0.5f, pos.y + (height - checkboxSize) * 0.5f), 
            ImVec2(pos.x + padding.x * 0.5f + checkboxSize, pos.y + checkboxSize + (height - checkboxSize) * 0.5f)
        );

        ImGuiID checkID = window->GetID("##checkbox");
        bool hovered, held;
        bool isPressed = ImGui::ButtonBehavior(bb, checkID, &hovered, &held, ImGuiButtonFlags_PressedOnClick);

        if (g.NavActivateId == id)
        {
            isPressed = true;
            g.NavActivateId = 0;
        }

        if (isPressed)
            *value = !(*value);

        ImGui::RenderNavCursor(bb, id);

        const ImU32 bgWidgetColor = ImGui::GetColorU32(ImGuiCol_FrameBg);
        window->DrawList->AddRectFilled(widgetRect.Min, widgetRect.Max, bgWidgetColor, rounding);

        const ImU32 checkColor = ImGui::GetColorU32(hovered ? ImGuiCol_FrameBgActive : ImGuiCol_FrameBgHovered);
        window->DrawList->AddRectFilled(checkRect.Min, checkRect.Max, checkColor, 2.0f);
        
        if (*value)
            ImGui::RenderCheckMark(window->DrawList, checkRect.Min + ImVec2(3, 3), IM_COL32(255, 255, 255, 255), checkboxSize - 6.0f);
        
        const ImVec2 textSize = ImGui::CalcTextSize(label);
        const ImVec2 checkTextPos = ImVec2(checkRect.Max.x + padding.x, pos.y + (height - textSize.y) * 0.5f);
        window->DrawList->AddText(checkTextPos, IM_COL32(255, 255, 255, 255), label);

        return isPressed;
    }

    static void _RenderSpacingBg()
    {
        const ImU32 bgWidgetColor = ImGui::GetColorU32(ImGuiCol_FrameBg);
        const ImVec2 spacing = ImGui::GetStyle().ItemSpacing;
        const float height = ImGui::GetFrameHeight();
        const float width = IM_TRUNC(ImGui::GetContentRegionAvail().x * 0.5f);

        ImVec2 pos = ImGui::GetCursorScreenPos() + ImVec2(width, height);
        ImVec2 size = { width, spacing.y };

        ImRect bb(pos, pos + size);
        ImGui::RenderFrame(bb.Min, bb.Max, bgWidgetColor, false, 0.0f);
    }

    static void _RenderButtonValue(const char* label, const ImVec4& color)
    {
        const float buttonSize = ImGui::GetFrameHeight();

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - buttonSize, 0.0f);

        //ImGui::PushStyleColor(ImGuiCol_Text, color);
        
        ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);
        //ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);

        ImGui::BeginDisabled();
        ImGui::ButtonEx(label, ImVec2(buttonSize, buttonSize));
        ImGui::EndDisabled();

        //ImGui::PopItemFlag();
        ImGui::PopItemFlag();
        
        //ImGui::PopStyleColor();
    }

    static void _RenderButtonRatio(const char* label, bool* value)
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        if (window->SkipItems)
            return;

        const float height = ImGui::GetFrameHeight();
        const ImVec2 padding = g.Style.FramePadding;

        const ImGuiID id = window->GetID(label);

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size = { height, height };
        ImRect bb(pos, pos + size);

        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, id, nullptr, ImGuiItemFlags_NoNav))
            return;

        bool hovered, held;
        if (ImGui::ButtonBehavior(bb, id, &hovered, &held, ImGuiButtonFlags_PressedOnClick))
            *value = !(*value);

        const ImU32 iconColor = hovered ? IM_COL32(255, 255, 255, 255) : IM_COL32(230, 230, 230, 255);
        const ImVec2 iconSize = ImVec2(height * 0.75f, height * 0.5f);
        const ImRect iconRect(
            ImVec2(pos.x + padding.x * 0.5f, pos.y + (height - iconSize.y) * 0.5f),
            ImVec2(pos.x + padding.x * 0.5f + iconSize.x, pos.y + iconSize.y + (height - iconSize.y) * 0.5f)
        );

        const float gapAngle = IM_PI * 0.35f;
        const int numSegments = 16;
        const float thickness = 1.5f;
        const float radius = iconSize.y * 0.5f - thickness;
        const ImVec2 centerL = ImVec2(iconRect.Min.x + radius, pos.y + height * 0.5f);
        const ImVec2 centerR = ImVec2(iconRect.Max.x - radius, pos.y + height * 0.5f);

        window->DrawList->PathArcTo(centerL, radius, gapAngle, IM_PI * 2.0f - gapAngle, numSegments);
        window->DrawList->PathStroke(iconColor, false, thickness);

        window->DrawList->PathArcTo(centerR, radius, IM_PI + gapAngle, IM_PI * 3.0f - gapAngle, numSegments);
        window->DrawList->PathStroke(iconColor, false, thickness);

        window->DrawList->AddLine(centerL, centerR, iconColor, thickness);

        if (*value == false)
            window->DrawList->AddLine(ImVec2(iconRect.Min.x, iconRect.Max.y), ImVec2(iconRect.Max.x, iconRect.Min.y), iconColor, thickness);
    }

    // --------------------------------------------------
    // --- CUSTOM IMGUI WIDGETS ----

    static bool FieldBool(const char* id, const char* label, bool* value, bool defaultValue)
    {
        const std::string itemLabel = std::string(id) + label;
        const ImGuiID itemID = ImGui::GetID(itemLabel.c_str());

        bool isPressed = false;
        bool isReset = false;

        ImGui::BeginGroup();

        isPressed |= _RenderField(itemID, label, (s_SelectedItem == itemID), (*value != defaultValue), &isReset);
        ImGui::SameLine(0.0f, 0.0f);
        isPressed |= _RenderCheckbox("Active", value);

        ImGui::EndGroup();

        if (isReset)
            *value = defaultValue;

        if (ImGui::IsItemClicked())
            s_SelectedItem = itemID;

        return isPressed;
    }
	
    static bool FieldInt(const char* id, const char* label, int* value, int defaultValue, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0)
    {
        const std::string itemLabel = std::string(id) + label;
        const std::string widgetLabel = std::string("##x") + itemLabel;
        const ImGuiID itemID = ImGui::GetID(itemLabel.c_str());

        bool isPressed = false;
        bool isReset = false;

        ImGui::BeginGroup();

        isPressed |= _RenderField(itemID, label, (s_SelectedItem == itemID), (*value != defaultValue), &isReset);
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        isPressed |= ImGui::InputInt(widgetLabel.c_str(), value, step, step_fast, flags);

        ImGui::EndGroup();

        if (isReset)
            *value = defaultValue;

        if (ImGui::IsItemClicked())
            s_SelectedItem = itemID;

        return isPressed;
    }

    static bool FieldFloat(const char* id, const char* label, float* value, float defaultValue, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0)
    {
        const std::string itemLabel = std::string(id) + label;
        const std::string widgetLabel = std::string("##x") + itemLabel;
        const ImGuiID itemID = ImGui::GetID(itemLabel.c_str());

        bool isPressed = false;
        bool isReset = false;

        ImGui::BeginGroup();

        isPressed |= _RenderField(itemID, label, (s_SelectedItem == itemID), (*value != defaultValue), &isReset);
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        isPressed |= ImGui::DragFloat(widgetLabel.c_str(), value, v_speed, v_min, v_max, format, flags);

        ImGui::EndGroup();

        if (isReset)
            *value = defaultValue;

        if (ImGui::IsItemClicked())
            s_SelectedItem = itemID;

        return isPressed;
    }

    static bool FieldFloat2(const char* id, const char* label, float* valueX, float* valueY, const glm::vec2& defaultValues, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0)
    {
        const std::string itemLabel = std::string(id) + label;
        const ImGuiID itemID = ImGui::GetID(itemLabel.c_str());

        const std::string itemLabelY = itemLabel + "Y";
        const ImGuiID itemID2 = ImGui::GetID(itemLabelY.c_str());

        const std::string widgetLabelX = std::string("##x") + itemLabel;
        const std::string widgetLabelY = std::string("##y") + itemLabel;

        const std::string buttonLabelX = std::string("X##") + itemLabel;
        const std::string buttonLabelY = std::string("Y##") + itemLabel;

        bool isPressed = false;
        bool isResetX = false;
        bool isResetY = false;

        ImGui::BeginGroup();

        _RenderSpacingBg();

        isPressed |= _RenderField(itemID, label, (s_SelectedItem == itemID), (*valueX != defaultValues.y), &isResetX);
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        isPressed |= ImGui::DragFloat(widgetLabelX.c_str(), valueX, v_speed, v_min, v_max, format, flags);
        _RenderButtonValue(buttonLabelX.c_str(), { 1.0f, 0.0f, 0.0f, 1.0f });

        isPressed |= _RenderField(itemID2, nullptr, false, (*valueY != defaultValues.x), &isResetY);
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        isPressed |= ImGui::DragFloat(widgetLabelY.c_str(), valueY, v_speed, v_min, v_max, format, flags);
        _RenderButtonValue(buttonLabelY.c_str(), { 0.0f, 1.0f, 0.0f, 1.0f });

        ImGui::EndGroup();

        if (isResetX)
            *valueX = defaultValues.x;
        if (isResetY)
            *valueY = defaultValues.y;

        if (ImGui::IsItemClicked())
            s_SelectedItem = itemID;

        return isPressed;
    }

	//static bool FieldFloat3();

	//static bool FieldFloat4();

    static bool FieldRatioFloat2(const char* id, const char* label, float* valueX, float* valueY, const glm::vec2& defaultValues, bool* isKeepRatio, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0)
    {
        const std::string itemLabel = std::string(id) + label;
        const ImGuiID itemID = ImGui::GetID(itemLabel.c_str());

        const std::string ratioLabel = std::string("##ratio") + itemLabel;

        ImGuiStorage* storage = ImGui::GetStateStorage();
        float ratio = storage->GetFloat(itemID, 0.0f);

        if (ratio == 0.0f)
        {
            ratio = (*valueX != 0.0f && *valueY != 0.0f) ? (*valueX / *valueY) : 1.0f;
            storage->SetFloat(itemID, ratio);
        }

        float prevValueX = *valueX;
        float prevValueY = *valueY;

        bool isPressed = FieldFloat2(id, label, valueX, valueY, defaultValues, v_speed, v_min, v_max, format, flags);

        ImGui::SameLine(ImGui::CalcTextSize(label).x + ImGui::GetStyle().FramePadding.x * 3.0f, 0.0f);
        _RenderButtonRatio(ratioLabel.c_str(), isKeepRatio);

        if (isKeepRatio && *isKeepRatio == true)
        {
            if (prevValueX != *valueX)
                *valueY = *valueX / ratio;

            else if (prevValueY != *valueY)
                *valueX = *valueY * ratio;
        }
        else if (*valueX != 0.0f && *valueY != 0.0f && (prevValueX != *valueX || prevValueY != *valueY))
        {
            ratio = *valueX / *valueY;
            storage->SetFloat(itemID, ratio);
        }

        return isPressed;
    }

    static bool FieldColor4(const char* id, const char* label, glm::vec4* value, const glm::vec4& defaultValue, ImGuiColorEditFlags flags = 0)
    {
        static glm::vec4 prevColor = *value;

        const std::string itemLabel = std::string(id) + label;
        const std::string widgetLabel = std::string("##color") + itemLabel;
        const std::string popupLabel = std::string("##popup") + itemLabel;
        const ImGuiID itemID = ImGui::GetID(itemLabel.c_str());

        bool isPressed = false;
        bool isReset = false;

        ImGui::BeginGroup();

        isPressed |= _RenderField(itemID, label, (s_SelectedItem == itemID), (*value != defaultValue), &isReset);
        ImGui::SameLine(0.0f, 0.0f);

        const ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight());
        if (ImGui::ColorButton(widgetLabel.c_str(), { value->r, value->g, value->b, value->a }, flags, size))
        {
            isPressed = true;
            ImGui::OpenPopup(popupLabel.c_str());
            prevColor = *value;
        }

        ImGui::EndGroup();

        if (isReset)
            *value = defaultValue;

        if (ImGui::IsItemClicked())
            s_SelectedItem = itemID;

        if (ImGui::BeginPopup(popupLabel.c_str()))
        {
            ImGui::ColorPicker4(label, glm::value_ptr(*value), flags, glm::value_ptr(prevColor));
            ImGui::EndPopup();
        }

        return isPressed;
    }

    static bool FieldColor3(const char* id, const char* label, glm::vec3* value, const glm::vec3& defaultValue, ImGuiColorEditFlags flags = 0)
    {
        glm::vec4 tmp = glm::vec4(*value, 1.0f);

        bool isPressed = FieldColor4(id, label, &tmp, glm::vec4(defaultValue, 1.0f), flags | ImGuiColorEditFlags_NoAlpha);

        *value = glm::vec3(tmp);

        return isPressed;
    }

    static bool FieldTexture(const char* id, const char* label, Ref<Texture2D>* texture, const Ref<Texture2D>& defaultTexture)
    {
        const std::string itemLabel = std::string(id) + label;
        const std::string widgetLabel = ((*texture) ? (*texture)->GetName() : "empty") + std::string("##") + itemLabel;
        const ImGuiID itemID = ImGui::GetID(itemLabel.c_str());

        ImGuiStorage* storage = ImGui::GetStateStorage();
        bool isOpen = storage->GetBool(itemID, false);

        bool isPressed = false;
        bool isReset = false;

        ImGui::BeginGroup();

        isPressed |= _RenderField(itemID, label, (s_SelectedItem == itemID), (*texture != defaultTexture), &isReset);
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        bool isButtonPressed = ImGui::Button(widgetLabel.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight()));

        ImGui::EndGroup();

        if (isButtonPressed)
        {
            isPressed = true;
            isOpen = !isOpen;

        }
        if (*texture == nullptr)
            isOpen = false;

        storage->SetFloat(itemID, isOpen);
        
        if (isReset)
            *texture = defaultTexture;

        if (ImGui::IsItemClicked())
            s_SelectedItem = itemID;

        if (ImGui::BeginDragDropTarget())
        {
            if (ImGui::AcceptDragDropPayload("ASSET_ITEM", ImGuiDragDropFlags_AcceptBeforeDelivery))
                ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_ITEM"))
            {
                const char* path = (const char*)payload->Data;
                std::filesystem::path texturePath(path);

                Ref<Texture2D> tex = Texture2D::Create(texturePath.filename().string(), texturePath.string());
                if (tex->IsLoaded())
                {
                    texture->reset();
                    *texture = tex;
                }
                else
                    DVS_WARN("Could not load texture {0}", texturePath.filename().string());
            }
            ImGui::EndDragDropTarget();
        }

        if (isOpen && *texture)
        {
            const float imageSize = ImGui::GetContentRegionAvail().x * 0.5f;
            const float textWidth = 250.0f;
            const ImVec4 textColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // yellow

            ImGui::Indent();

            ImGui::Image((*texture)->GetRenderID(), { imageSize, imageSize }, { 0,1 }, { 1,0 });
            
            ImGui::Text("ID:   ");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(textColor, "%d", (*texture)->GetRenderID());

            ImGui::Text("Size: ");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(textColor, "%d", (*texture)->GetWidth());
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text("x");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(textColor, "%d", (*texture)->GetHeight());

            ImGui::Text("Path: ");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, textColor);
            ImGui::TextWrapped("%s", (*texture)->GetPath().c_str());
            ImGui::PopStyleColor();

            ImGui::Unindent();
            ImGui::Separator();
        }

        return isPressed;
    }

}
