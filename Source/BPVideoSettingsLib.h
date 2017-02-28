// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "BPVideoSettingsLib.generated.h"

#define MIN_SCREEN_WIDTH 1024
#define MIN_SCREEN_HEIGHT 768

UENUM(BlueprintType)
enum ECustomWindowMode
{
	/** The window is in true fullscreen mode */
	Fullscreen,
	/** The window has no border and takes up the entire area of the screen */
	WindowedFullscreen,
	/** The window has a border and may not take up the entire screen area */
	Windowed,
	
	None
};

/**
* Custom Blueprint Function Library.
*/
UCLASS()
class HOMEINVASION_API UBPVideoSettingsLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	public:
		// Get a list of screen resolutions supported on this machine
		UFUNCTION(BlueprintPure, Category = "Video Settings")
		static bool GetSupportedScreenResolutions(TArray<FString>& Resolutions);

		// Get currently set screen resolution
		UFUNCTION(BlueprintPure, Category = "Video Settings")
		static FString GetScreenResolution();

		// Get currently set screen resolution
		UFUNCTION(BlueprintPure, Category = "Video Settings")
		static FVector2D GetScreenResolutionVector();

		// Get currently set screen resolution
		UFUNCTION(BlueprintPure, Category = "Video Settings")
		static FString GetLastConfirmedScreenResolution();

		// Get currently set screen resolution
		UFUNCTION(BlueprintPure, Category = "Video Settings")
		static FVector2D GetLastConfirmedScreenResolutionVector();

		// Check whether or not we are currently running in fullscreen mode
		UFUNCTION(BlueprintPure, Category = "Video Settings")
		static bool IsInFullscreen();

		// Get the current window mode
		UFUNCTION(BlueprintPure, Category = "Video Settings")
		static ECustomWindowMode GetWindowMode();

		// Get the current window mode
		UFUNCTION(BlueprintPure, Category = "Video Settings")
		static ECustomWindowMode GetLastConfirmedWindowMode();

		// Set the desired screen resolution (does not change it yet)
		UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool SetScreenResolution(const int32 Width, const int32 Height, const bool Fullscreen);

		// Set the desired screen resolution (does not change it yet)
		UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool SetScreenResolutionAndMode(const int32 Width, const int32 Height, const ECustomWindowMode InWindowMode);

		// Change the current screen resolution
		UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool ChangeScreenResolution(const int32 Width, const int32 Height, const bool Fullscreen);

		// Get the current video quality settings
		UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool GetVideoQualitySettings(int32& AntiAliasing, int32& Effects, int32& PostProcess, int32& Resolution, int32& Shadow, int32& Texture, int32& ViewDistance);

		// Set the quality settings (not applied nor saved yet)
		UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool SetVideoQualitySettings(const int32 AntiAliasing = 3, const int32 Effects = 3, const int32 PostProcess = 3,	const int32 Resolution = 100, const int32 Shadow = 3, const int32 Texture = 3, const int32 ViewDistance = 3);

		// Check whether or not we have vertical sync enabled
		UFUNCTION(BlueprintPure, Category = "Video Settings")
		static bool IsVSyncEnabled();

		// Set the vertical sync flag
		UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool SetVSyncEnabled(const bool VSync);


		// Apply current video mode (resolution and fullscreen/windowed) as well as quality settings
		// Does not save!
		UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool ApplyVideoModeAndQuality();

		// Save current video mode (resolution and fullscreen/windowed) as well as quality settings
		UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool SaveVideoModeAndQuality();

		// Revert back to the .ini settings
		UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool RevertVideoModeAndQuality();

		// Reload the settings
		UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool ReloadSettings(bool ForceReload);

		// Set the settings back to default values
		UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool SetToDefault();

		// Set the settings back to default values, not the standard which sets values to zero values.
		UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool SetToDefaultCustom(int ResolutionX, int ResolutionY, ECustomWindowMode WindowMode);

		// Revert to original video settings
		UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool RevertVideoMode();

	private:
		// Try to get the GameUserSettings object from the engine
		static UGameUserSettings* GetGameUserSettings();
};
