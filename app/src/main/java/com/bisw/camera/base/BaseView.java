package com.bisw.camera.base;

public interface BaseView {
    // Interface for outside called
    void Pause();

    void Resume();

    void Close();

    ICameraManager getCameraManager();
}
