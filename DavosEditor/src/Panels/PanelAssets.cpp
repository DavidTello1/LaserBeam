#include "PanelAssets.h"

#include <imgui/imgui.h>

namespace Davos {

	static const std::filesystem::path s_AssetsPath = "assets";

	void PanelAssets::OnInit()
	{
		m_BaseDirectory = s_AssetsPath;

		m_DirectoryIcon = Texture2D::Create("res/icons/FolderIcon.png");
		m_FileIcon = Texture2D::Create("res/icons/FileIcon.png");
	}

	void PanelAssets::OnImGuiRender()
	{
		ImGui::Begin("Assets");
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 16.0f);

		std::filesystem::directory_entry root(m_BaseDirectory);
		_DrawNode(root, true);

		ImGui::PopStyleVar();
		ImGui::End();
	}

	// --------------------------------------------------
	void PanelAssets::_DrawNode(const std::filesystem::directory_entry& entry, bool isOpenDefault)
	{
		static const ImVec2 iconSize = ImVec2(16, 16);

		const bool isDir = entry.is_directory();
		const std::filesystem::path& path = entry.path();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;

		if (isOpenDefault)
			flags |= ImGuiTreeNodeFlags_DefaultOpen;

		if (!isDir)
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		if (m_SelectedNode == path)
			flags |= ImGuiTreeNodeFlags_Selected;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 12.0f));
		ImGui::PushID(path.string().c_str());

		bool isOpen = ImGui::TreeNodeEx("##node", flags);
		if (ImGui::IsItemClicked())
			m_SelectedNode = path;

		ImGui::SameLine(0.0f, 1.0f);
		ImGui::Image((ImTextureID)(isDir ? m_DirectoryIcon->GetRenderID() : m_FileIcon->GetRenderID()), iconSize, { 0,1 }, { 1,0 });
		ImGui::SameLine(0.0f, 4.0f);
		ImGui::TextUnformatted(path.filename().string().c_str());

		if (isDir && isOpen)
		{
			std::vector<std::filesystem::directory_entry> entries = _SortDirectory(path);
			for (auto& entry : entries)
				_DrawNode(entry);

			ImGui::TreePop();
		}

		ImGui::PopID();
		ImGui::PopStyleVar();
	}

	std::vector<std::filesystem::directory_entry> PanelAssets::_SortDirectory(const std::filesystem::path& directory)
	{
		std::vector<std::filesystem::directory_entry> entries;

		for (auto& e : std::filesystem::directory_iterator(directory))
			entries.push_back(e);

		std::sort(entries.begin(), entries.end(),
			[](auto& a, auto& b)
		{
			if (a.is_directory() != b.is_directory())
				return a.is_directory() > b.is_directory();
			return a.path().filename() < b.path().filename();
		});

		return entries;
	}

}
