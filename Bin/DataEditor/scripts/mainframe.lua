-- mainframe.lua

function e_CreateMainMenu()
	-- file
	local itemFile = E_MainFrame:AddMainMenuItem((PX2_LM:GetValue("File")))
	E_MainFrame:AddMenuItem(itemFile, PX2_LM:GetValue("NewProject").."\tCtrl-N", "e_OnNewProject()", "newproject")
	E_MainFrame:AddMenuItem(itemFile, PX2_LM:GetValue("Open").."\tCtrl-O", "e_OnOpenProject()", "openproject")
	E_MainFrame:AddMenuItem(itemFile, PX2_LM:GetValue("Save").."\tCtrl-S", "e_OnSaveProject()", "saveproject")
	E_MainFrame:AddMenuItem(itemFile, PX2_LM:GetValue("Close"), "e_OnCloseProject()", "closeproject")
	E_MainFrame:AddSeparater(itemFile)
	E_MainFrame:AddMenuItem(itemFile, PX2_LM:GetValue("NewScene"), "e_OnNewScene()", "newscene")
	E_MainFrame:AddMenuItem(itemFile, PX2_LM:GetValue("Open"), "e_OnOpenScene()", "openscene")
	E_MainFrame:AddMenuItem(itemFile, PX2_LM:GetValue("Save"), "e_OnSaveScene()", "savescene")
	E_MainFrame:AddMenuItem(itemFile, PX2_LM:GetValue("SaveAs"), "e_OnSaveAsScene()", "saveasscene")
	E_MainFrame:AddMenuItem(itemFile, PX2_LM:GetValue("Close"), "e_OnCloseScene()", "closescene")
	E_MainFrame:AddSeparater(itemFile)
	E_MainFrame:AddMenuItem(itemFile, PX2_LM:GetValue("Exit").."\tAlt-X", "e_OnExit()")
	--edit
	local itemEdit = E_MainFrame:AddMainMenuItem((PX2_LM:GetValue("Edit")))
	E_MainFrame:AddMenuItem(itemEdit, PX2_LM:GetValue("OnDo").."\tCtrl-Z", "e_OnUnDo()", "edit")
	E_MainFrame:AddMenuItem(itemEdit, PX2_LM:GetValue("ReDo").."\tCtrl-Y", "e_OnReDo()", "edit")
	E_MainFrame:AddSeparater(itemEdit)
	E_MainFrame:AddMenuItem(itemEdit, PX2_LM:GetValue("Copy").."\tCtrl-C", "e_OnCopy()", "edit")
	E_MainFrame:AddMenuItem(itemEdit, PX2_LM:GetValue("Paste").."\tCtrl-V", "e_OnPaste()", "edit")
	E_MainFrame:AddMenuItem(itemEdit, PX2_LM:GetValue("Delete").."\tDelete", "e_OnDelete()", "edit")
	E_MainFrame:AddSeparater(itemEdit)
	E_MainFrame:AddMenuItem(itemEdit, PX2_LM:GetValue("Import").."\tCtrl-I", "e_OnImport()", "edit")
	E_MainFrame:AddMenuItem(itemEdit, PX2_LM:GetValue("Export").."\tCtrl-E", "e_OnExport()", "edit")
	E_MainFrame:AddSeparater(itemEdit)
	E_MainFrame:AddMenuItem(itemEdit, PX2_LM:GetValue("Select"), "e_OnSelect()", "edit")
	E_MainFrame:AddMenuItem(itemEdit, PX2_LM:GetValue("Translate"), "e_OnTranslate()", "edit")
	E_MainFrame:AddMenuItem(itemEdit, PX2_LM:GetValue("Rotate"), "e_OnRotate()", "edit")
	E_MainFrame:AddMenuItem(itemEdit, PX2_LM:GetValue("Scale"), "e_OnScale()", "edit")
	E_MainFrame:AddSeparater(itemEdit)
	E_MainFrame:AddMenuItem(itemEdit, PX2_LM:GetValue("PlayAndStop"), "e_AnimPlayStop()", "edit")
	E_MainFrame:AddMenuItem(itemEdit, PX2_LM:GetValue("ResetPlay").."\tAlt-Space", "e_AnimResetPlay()", "edit")
	--itemview
	local itemView = E_MainFrame:AddMainMenuItem((PX2_LM:GetValue("View")))
	local subMenuWindow = NirMan:AddSubMenu(itemView, (PX2_LM:GetValue("Window")))
	NirMan:AddMenuItem(subMenuWindow, PX2_LM:GetValue("Project"), "e_ShowWindow('Project')")
	NirMan:AddMenuItem(subMenuWindow, PX2_LM:GetValue("Stage"), "e_ShowWindow('Stage')")	
	NirMan:AddMenuItem(subMenuWindow, PX2_LM:GetValue("TimeLine"), "e_ShowWindow('TimeLine')")	
	NirMan:AddMenuItem(subMenuWindow, PX2_LM:GetValue("ResView"), "e_ShowWindow('ResView')")	
	NirMan:AddMenuItem(subMenuWindow, PX2_LM:GetValue("InspView"), "e_ShowWindow('InspView')")
	NirMan:AddMenuItem(subMenuWindow, PX2_LM:GetValue("ConsoleView"), "e_ShowWindow('ConsoleView')")	
	NirMan:AddMenuItem(subMenuWindow, PX2_LM:GetValue("PreView"), "e_ShowWindow('PreView')")	
	
	E_MainFrame:AddMenuItem(itemView, PX2_LM:GetValue("StageCenter").."\tCtrl-Enter", "e_OnStageCenter()")
	E_MainFrame:AddSeparater(itemView)
	E_MainFrame:AddMenuItem(itemView, PX2_LM:GetValue("PerspectiveCamera").."\tF5", "e_OnPerspectiveCamera()", "edit")
	E_MainFrame:AddMenuItem(itemView, PX2_LM:GetValue("TopViewCamera").."\tF6", "e_OnTopViewCamera()", "edit")
	--debug
	local itemDebug = E_MainFrame:AddMainMenuItem((PX2_LM:GetValue("Debug")))
	E_MainFrame:AddMenuItem(itemDebug, PX2_LM:GetValue("EditorSimulate"), "e_OnEditorSimulate()", "edit")
	E_MainFrame:AddMenuItem(itemDebug, PX2_LM:GetValue("EditorPlay"), "e_OnEditorPlay()", "edit")
	E_MainFrame:AddMenuItem(itemDebug, PX2_LM:GetValue("EditorStop"), "e_OnEditorStop()", "edit")
	E_MainFrame:AddMenuItem(itemDebug, PX2_LM:GetValue("WindowPlay").."\tCtrl-G", "e_OnPlay()", "edit")
	E_MainFrame:AddSeparater(itemDebug)
	E_MainFrame:AddMenuItem(itemDebug, PX2_LM:GetValue("PlayConfig"), "e_OnPlayConfig()", "edit")
	--tool
	local itemTool = E_MainFrame:AddMainMenuItem((PX2_LM:GetValue("Tool")))
	E_MainFrame:AddMenuItem(itemTool, PX2_LM:GetValue("VersionWindows"), "e_OnVersionWindows()", "edit")
	E_MainFrame:AddMenuItem(itemTool, PX2_LM:GetValue("VersionAndroid"), "e_OnVersionAndroid()", "edit")
	E_MainFrame:AddMenuItem(itemTool, PX2_LM:GetValue("VersionIOS"), "e_OnVersionIOS()", "edit")
	E_MainFrame:AddSeparater(itemTool)
	E_MainFrame:AddMenuItem(itemTool, PX2_LM:GetValue("Setting"), "e_OnSetting()", "")
	--help
	local itemHelp = E_MainFrame:AddMainMenuItem((PX2_LM:GetValue("Help")))
	E_MainFrame:AddMenuItem(itemHelp, PX2_LM:GetValue("About"), "e_OnAbout()", "")
end