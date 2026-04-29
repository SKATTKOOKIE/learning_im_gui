#include "components/JointStatusTable/JointStatusTable.h"
#include <imgui.h>

JointStatusTable::JointStatusTable()
{
    for (int i = 0; i <= NUM_JOINTS; i++)
        jointStatuses_[i].joint = i;
}

void JointStatusTable::HandleTelemetry(const std::string& message)
{
    JointStatusParser::JointStatus status;
    if (JointStatusParser::TryParse(message, status))
    {
        if (status.joint >= 1 && status.joint <= NUM_JOINTS)
            jointStatuses_[status.joint] = status;
    }
}

void JointStatusTable::Render()
{
    ImGui::Begin("Joint Status###JointStatusTable");

    ImGuiTableFlags flags = ImGuiTableFlags_Borders
                          | ImGuiTableFlags_RowBg
                          | ImGuiTableFlags_SizingFixedFit
                          | ImGuiTableFlags_ScrollY;

    if (ImGui::BeginTable("StatusTable", 10, flags))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Joint",       ImGuiTableColumnFlags_WidthFixed, 90.0f);
        ImGui::TableSetupColumn("Name",        ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Op Mode",     ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Ctrl Mode",   ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Regen",       ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Brake",       ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Moving",      ImGuiTableColumnFlags_WidthFixed, 55.0f);
        ImGui::TableSetupColumn("Halls",       ImGuiTableColumnFlags_WidthFixed, 45.0f);
        ImGui::TableSetupColumn("Temp Warn",   ImGuiTableColumnFlags_WidthFixed, 70.0f);
        ImGui::TableSetupColumn("RT Ctrl",     ImGuiTableColumnFlags_WidthFixed, 55.0f);
        ImGui::TableHeadersRow();

        for (int i = 1; i <= NUM_JOINTS; i++)
        {
            const auto& s = jointStatuses_[i];
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("J%d", i);

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", JointStatusParser::JointName(i));

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", JointStatusParser::OperationalModeName(s.operational_mode));

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%s", JointStatusParser::ControlModeName(s.control_mode));

            ImGui::TableSetColumnIndex(4);
            ImGui::Text("%d", s.regeneration_mode);

            auto boolCell = [](bool val) {
                if (val)
                {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
                        IM_COL32(180, 60, 60, 100));
                    ImGui::Text("YES");
                }
                else
                {
                    ImGui::Text("---");
                }
            };

            ImGui::TableSetColumnIndex(5);  boolCell(s.brake_override);
            ImGui::TableSetColumnIndex(6);  boolCell(s.movement_in_progress);
            ImGui::TableSetColumnIndex(7);  boolCell(s.use_halls);
            ImGui::TableSetColumnIndex(8);  boolCell(s.temperature_warning);
            ImGui::TableSetColumnIndex(9);  boolCell(s.realtime_control_mode);
        }

        ImGui::EndTable();
    }

    ImGui::End();
}