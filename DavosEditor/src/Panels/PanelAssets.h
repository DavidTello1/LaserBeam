#pragma once
#include <Davos.h>

#include <filesystem>

namespace Davos {

	class PanelAssets
	{
	public:
		PanelAssets() = default;
		~PanelAssets() = default;

		void OnInit();
		void OnImGuiRender();

	private:
		void _DrawNode(const std::filesystem::directory_entry& entry, bool isOpenDefault = false);
		std::vector<std::filesystem::directory_entry> _SortDirectory(const std::filesystem::path& directory);

	private:
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_SelectedNode;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
	};

}
