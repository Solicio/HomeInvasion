// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "BPVideoSettingsLib.h"

// Get a list of screen resolutions supported by video adapter
bool UBPVideoSettingsLib::GetSupportedScreenResolutions(TArray<FString>& Resolutions)
{
	FScreenResolutionArray ResolutionsArray;

	if (RHIGetAvailableResolutions(ResolutionsArray, true))  // needs the "RHI" dependency
	{
		for (const FScreenResolutionRHI& Resolution : ResolutionsArray)
		{
			if (Resolution.Width < MIN_SCREEN_WIDTH || Resolution.Height < MIN_SCREEN_HEIGHT)
			{
				continue;
			}

			FString Str = FString::FromInt(Resolution.Width) + "x" + FString::FromInt(Resolution.Height);
			Resolutions.AddUnique(Str);
		}

		return true;
	}

	return false;  // failed to obtain screen resolutions
}


// Get currently set screen resolution
FString UBPVideoSettingsLib::GetScreenResolution()
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return FString("");
	}

	FIntPoint Resolution = Settings->GetScreenResolution();
	return FString::FromInt(Resolution.X) + "x" + FString::FromInt(Resolution.Y);
}

FVector2D UBPVideoSettingsLib::GetScreenResolutionVector()
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return FVector2D(0.0f, 0.0f);
	}

	FIntPoint Resolution = Settings->GetScreenResolution();
	return FVector2D(Resolution.X, Resolution.Y);
}

FString UBPVideoSettingsLib::GetLastConfirmedScreenResolution()
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return FString("");
	}

	FIntPoint Resolution = Settings->GetLastConfirmedScreenResolution();
	return FString::FromInt(Resolution.X) + "x" + FString::FromInt(Resolution.Y);
}

FVector2D UBPVideoSettingsLib::GetLastConfirmedScreenResolutionVector()
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return FVector2D(0.0f, 0.0f);
	}

	FIntPoint Resolution = Settings->GetLastConfirmedScreenResolution();
	return FVector2D(Resolution.X, Resolution.Y);
}

// Check whether or not we are currently running in fullscreen mode
bool UBPVideoSettingsLib::IsInFullscreen()
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	return Settings->GetFullscreenMode() == EWindowMode::Fullscreen;
}

// Return the current window mode
ECustomWindowMode UBPVideoSettingsLib::GetWindowMode()
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return ECustomWindowMode::None;
	}

	switch (Settings->GetFullscreenMode())
	{
	case EWindowMode::Fullscreen:
			return ECustomWindowMode::Fullscreen;
			break;

		case EWindowMode::WindowedFullscreen:
			return ECustomWindowMode::WindowedFullscreen;
			break;

		case EWindowMode::Windowed:
			return ECustomWindowMode::Windowed;
			break;
	}
	return ECustomWindowMode::None;
}

ECustomWindowMode UBPVideoSettingsLib::GetLastConfirmedWindowMode()
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return ECustomWindowMode::None;
	}

	switch (Settings->GetLastConfirmedFullscreenMode())
	{
	case EWindowMode::Fullscreen:
		return ECustomWindowMode::Fullscreen;
		break;

	case EWindowMode::WindowedFullscreen:
		return ECustomWindowMode::WindowedFullscreen;
		break;

	case EWindowMode::Windowed:
		return ECustomWindowMode::Windowed;
		break;
	}
	return ECustomWindowMode::None;
}

// Set the desired screen resolution (does not change it yet)
bool UBPVideoSettingsLib::SetScreenResolution(const int32 Width, const int32 Height, const bool Fullscreen)
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	Settings->SetScreenResolution(FIntPoint(Width, Height));
	Settings->SetFullscreenMode(Fullscreen ? EWindowMode::Fullscreen : EWindowMode::Windowed);
	return true;
}

bool UBPVideoSettingsLib::SetScreenResolutionAndMode(const int32 Width, const int32 Height, const ECustomWindowMode InWindowMode)
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	Settings->SetScreenResolution(FIntPoint(Width, Height));

	// Set the window mode
	switch (InWindowMode)
	{
		case ECustomWindowMode::Fullscreen:
			Settings->SetFullscreenMode(EWindowMode::Fullscreen);
			break;

		case ECustomWindowMode::WindowedFullscreen:
			Settings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
			break;

		case ECustomWindowMode::Windowed:
			Settings->SetFullscreenMode(EWindowMode::Windowed);
			break;
	}

	
	return true;
}


// Change the current screen resolution
bool UBPVideoSettingsLib::ChangeScreenResolution(const int32 Width, const int32 Height, const bool Fullscreen)
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	EWindowMode::Type WindowMode = Fullscreen ? EWindowMode::Fullscreen : EWindowMode::Windowed;
	Settings->RequestResolutionChange(Width, Height, WindowMode, false);
	return true;
}


// Get the current video quality settings
bool UBPVideoSettingsLib::GetVideoQualitySettings(int32& AntiAliasing, int32& Effects, int32& PostProcess, int32& Resolution, int32& Shadow, int32& Texture, int32& ViewDistance)
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	AntiAliasing = Settings->ScalabilityQuality.AntiAliasingQuality;
	Effects = Settings->ScalabilityQuality.EffectsQuality;
	PostProcess = Settings->ScalabilityQuality.PostProcessQuality;
	Resolution = Settings->ScalabilityQuality.ResolutionQuality;
	Shadow = Settings->ScalabilityQuality.ShadowQuality;
	Texture = Settings->ScalabilityQuality.TextureQuality;
	ViewDistance = Settings->ScalabilityQuality.ViewDistanceQuality;
	return true;
}


// Set the quality settings (not applied nor saved yet)
bool UBPVideoSettingsLib::SetVideoQualitySettings(const int32 AntiAliasing, const int32 Effects, const int32 PostProcess, const int32 Resolution, const int32 Shadow, const int32 Texture, const int32 ViewDistance)
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	Settings->ScalabilityQuality.AntiAliasingQuality = AntiAliasing;
	Settings->ScalabilityQuality.EffectsQuality = Effects;
	Settings->ScalabilityQuality.PostProcessQuality = PostProcess;
	Settings->ScalabilityQuality.ResolutionQuality = Resolution;
	Settings->ScalabilityQuality.ShadowQuality = Shadow;
	Settings->ScalabilityQuality.TextureQuality = Texture;
	Settings->ScalabilityQuality.ViewDistanceQuality = ViewDistance;
	return true;
}


// Check whether or not we have vertical sync enabled
bool UBPVideoSettingsLib::IsVSyncEnabled()
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	return Settings->IsVSyncEnabled();
}


// Set the vertical sync flag
bool UBPVideoSettingsLib::SetVSyncEnabled(const bool VSync)
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	Settings->SetVSyncEnabled(VSync);
	return true;
}


// Apply current video mode (resolution and fullscreen/windowed) as well as quality settings
bool UBPVideoSettingsLib::ApplyVideoModeAndQuality()
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	Settings->ApplyResolutionSettings(false);
	Settings->ApplyNonResolutionSettings();

	return true;
}

// Save current video mode (resolution and fullscreen/windowed)
bool UBPVideoSettingsLib::SaveVideoModeAndQuality()
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	Settings->ConfirmVideoMode();
	Settings->SaveSettings();

	return true;
}


// Revert back to the .ini settings
bool UBPVideoSettingsLib::RevertVideoModeAndQuality()
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	Settings->ResetToCurrentSettings();

	//if (Settings->IsVSyncDirty())
	//	Settings->SetVSyncEnabled(!Settings->IsVSyncEnabled());
	
	ApplyVideoModeAndQuality();

	return true;
}

bool UBPVideoSettingsLib::ReloadSettings(bool ForceReload)
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	Settings->LoadSettings(ForceReload);
	return true;
}

bool UBPVideoSettingsLib::SetToDefault()
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	Settings->SetToDefaults();
	return true;
}

bool UBPVideoSettingsLib::SetToDefaultCustom(int ResolutionX, int ResolutionY, ECustomWindowMode WindowMode)
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	Settings->SetScreenResolution(FIntPoint(ResolutionX, ResolutionY));

	switch (WindowMode)
	{
		case Fullscreen:
			Settings->SetFullscreenMode(EWindowMode::Fullscreen);
			break;

		case WindowedFullscreen:
			Settings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
			break;

		case Windowed:
			Settings->SetFullscreenMode(EWindowMode::Windowed);
			break;
	};

	Settings->ScalabilityQuality.SetFromSingleQualityLevel(3);

	return true;
}



// Revert to original video settings
bool UBPVideoSettingsLib::RevertVideoMode()
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	Settings->RevertVideoMode();
	return true;
}


//---- PRIVATE METHODS -------------------------------------------------------------------------------

// Try to get the GameUserSettings object from the engine
UGameUserSettings* UBPVideoSettingsLib::GetGameUserSettings()
{
	if (GEngine != nullptr)
	{
		return GEngine->GameUserSettings;
	}

	return nullptr;
}



