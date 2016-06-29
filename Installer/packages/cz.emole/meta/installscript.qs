function Component()
{
    component.loaded.connect(this, addRegisterFileCheckBox);
    component.unusualFileType = "mogr";
}

addRegisterFileCheckBox = function()
{
    // don't show when updating or uninstalling
    if (installer.isInstaller()) {
        installer.addWizardPageItem(component, "RegisterFileCheckBoxForm", QInstaller.TargetDirectory);
        component.userInterface("RegisterFileCheckBoxForm").RegisterFileCheckBox.text =
            component.userInterface("RegisterFileCheckBoxForm").RegisterFileCheckBox.text + component.unusualFileType;
    }
}

// here we are creating the operation chain which will be processed at the real installation part later
Component.prototype.createOperations = function()
{
    // call default implementation to actually install the registeredfile
    component.createOperations();

    var isRegisterFileChecked = component.userInterface("RegisterFileCheckBoxForm").RegisterFileCheckBox.checked;
    if (installer.value("os") === "win") {
        var iconId = 0;
        var moleGraphPath = "@TargetDir@/MoleGraph.exe";
        component.addOperation("RegisterFileType",
                               component.unusualFileType,
                               moleGraphPath + " '%1'",
                               "MoleGraph file type",
                               "text/plain",
                               moleGraphPath + "," + iconId,
                               "ProgId=QtProject.QtInstallerFramework." + component.unusualFileType);
    }
    
    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/MoleGraph.exe", "@StartMenuDir@/MoleGraph.lnk",
            "workingDirectory=@TargetDir@", "@TargetDir@/MoleGraph.exe",
            "iconId=1");
    }
}
