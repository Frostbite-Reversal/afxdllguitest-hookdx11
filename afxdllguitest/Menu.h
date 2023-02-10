#pragma once
#include<tchar.h>
bool mainBeginSate = TRUE;
bool tab_bar_flags = TRUE;
bool checkBoxFalgs_1 = FALSE;

void loadTab1()
{

	if (ImGui::BeginTabItem("SCRIPT - 功能"))
	{

		if (ImGui::Button("游戏初始化1"))
		{
			checkBoxFalgs_1 = !checkBoxFalgs_1;
		}

		if (ImGui::Checkbox("开启飞天", &checkBoxFalgs_1))
		{
			//  点击事件执行
		}

		if (checkBoxFalgs_1)
		{
			ImGui::Text("欢迎使用IMGUI 窗口3");
			ImGui::SameLine();
			ImGui::Text("欢迎使用IMGUI 窗口4");
		}
		ImGui::EndTabItem();
	}


}
void loadTab2()
{
	if (ImGui::BeginTabItem("SCRIPT - 特殊"))
	{


		if (ImGui::Button("游戏初始化2"))
		{
			checkBoxFalgs_1 = !checkBoxFalgs_1;
		}

		if (ImGui::Checkbox("开启飞天", &checkBoxFalgs_1))
		{
			//  点击事件执行
		}

		if (checkBoxFalgs_1)
		{
			ImGui::Text("欢迎使用IMGUI 窗口3");
			ImGui::SameLine();
			ImGui::Text("欢迎使用IMGUI 窗口4");
		}
		ImGui::EndTabItem();
	}

}
void loadTab3()
{
	if (ImGui::BeginTabItem("SCRIPT - 其他"))
	{
		if (ImGui::Button("游戏初始化3"))
		{
			checkBoxFalgs_1 = !checkBoxFalgs_1;
		}

		if (ImGui::Checkbox("开启飞天", &checkBoxFalgs_1))
		{

		}
		ImGui::Columns(1);
		ImGui::Separator();

		ImGui::Text("With border:");
		ImGui::Columns(4, "mycolumns"); // 4-ways, with border
		ImGui::Separator();
		ImGui::Text("ID"); ImGui::NextColumn();
		ImGui::Text("Name"); ImGui::NextColumn();
		ImGui::Text("Path"); ImGui::NextColumn();
		ImGui::Text("Hovered"); ImGui::NextColumn();
		ImGui::Separator();
		const char* names[3] = { "One", "Two", "Three" };
		const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
		static int selected = -1;
		for (int i = 0; i < 3; i++)
		{
			char label[32];
			sprintf(label, "%04d", i);
			if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
				selected = i;
			bool hovered = ImGui::IsItemHovered();
			ImGui::NextColumn();
			ImGui::Text(names[i]); ImGui::NextColumn();
			ImGui::Text(paths[i]); ImGui::NextColumn();
			ImGui::Text("%d", hovered); ImGui::NextColumn();
		}
		ImGui::Columns(1);
		ImGui::Separator();
		if (checkBoxFalgs_1)
		{
			ImGui::Text("欢迎使用IMGUI 窗口3");
			ImGui::SameLine();
			ImGui::Text("欢迎使用IMGUI 窗口4");
		}
		ImGui::EndTabItem();
	}

}
void loadTab4()
{
	if (ImGui::BeginTabItem("SCRIPT - 说明"))
	{

		if (ImGui::Button("游戏初始化4"))
		{
			checkBoxFalgs_1 = !checkBoxFalgs_1;
		}

		if (ImGui::Checkbox("开启飞天", &checkBoxFalgs_1))
		{
			//  点击事件执行
		}

		if (checkBoxFalgs_1)
		{
			ImGui::Text("欢迎使用IMGUI 窗口3");
			ImGui::SameLine();
			ImGui::Text("欢迎使用IMGUI 窗口4");
		}
		ImGui::EndTabItem();
	}

}

void MyMenu()
{

	// 窗口大小
	ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(350, 450));
	ImGui::Begin("测试游戏辅助窗口", &mainBeginSate);

	if (ImGui::BeginTabBar("tab1", tab_bar_flags))
	{
		loadTab1();
		loadTab2();
		loadTab3();
		loadTab4();
	}
	ImGui::EndTabBar();
	ImGui::End();
}

void Theme()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowBorderSize = 0;
	
	style->WindowMinSize = ImVec2(0, 0);
	style->WindowTitleAlign = ImVec2(0.5, 0.5);
	style->SelectableTextAlign = ImVec2(0.5, 0.5);
	style->SelectableTextAlign=ImVec2(0.5, 0.5);
	style->FramePadding = ImVec2(8, 6);

	style->Colors[ImGuiCol_TitleBg] = ImColor(255, 101, 53, 255);
	style->Colors[ImGuiCol_TitleBgActive] = ImColor(255, 101, 53, 255);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 0, 0, 130);

	style->Colors[ImGuiCol_Button] = ImColor(31, 30, 31, 255);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(31, 30, 31, 255);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(41, 40, 41, 255);

	style->Colors[ImGuiCol_Separator] = ImColor(70, 70, 70, 255);
	style->Colors[ImGuiCol_SeparatorActive] = ImColor(76, 76, 76, 255);
	style->Colors[ImGuiCol_SeparatorHovered] = ImColor(76, 76, 76, 255);

	style->Colors[ImGuiCol_FrameBg] = ImColor(37, 36, 37, 255);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(37, 36, 37, 255);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(37, 36, 37, 255);

	style->Colors[ImGuiCol_Header] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_HeaderActive] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_HeaderHovered] = ImColor(46, 46, 46, 255);
}