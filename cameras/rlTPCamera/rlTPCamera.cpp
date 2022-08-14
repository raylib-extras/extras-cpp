/**********************************************************************************************
*
*   raylibExtras * Utilities and Shared Components for Raylib
*
*   TPOrbitCamera * Third Person Camera Example
*
*   LICENSE: MIT
*
*   Copyright (c) 2021 Jeffery Myers
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all
*   copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*   SOFTWARE.
*
**********************************************************************************************/


#include "rlTPCamera.h"
#include "raylib.h"
#include "rlgl.h"
#include <stdlib.h>
#include <math.h>

static void ResizeTPOrbitCameraView(rlTPCamera* camera)
{
    if (camera == NULL)
        return;

    float width = (float)GetScreenWidth();
    float height = (float)GetScreenHeight();

    camera->FOV.y = camera->ViewCamera.fovy;

    if (height != 0)
        camera->FOV.x = camera->FOV.y * (width / height);
}

rlTPCamera::rlTPCamera() : ControlsKeys{ 'W', 'S', 'D', 'A', 'E', 'Q', KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_LEFT_SHIFT }
{

}

void rlTPCamera::Setup(float fovY, Vector3 position)
{
    MoveSpeed = Vector3{ 3,3,3 };
    TurnSpeed = Vector2{ 90,90 };

    MouseSensitivity = 600;

    MinimumViewY = -89.995f;
    MaximumViewY = 0.0f;

    PreviousMousePosition = GetMousePosition();
    Focused = IsWindowFocused();

    CameraPullbackDistance = 5;

    ViewAngles = Vector2{ 0,0 };

    CameraPosition = position;
    FOV.y = fovY;

    ViewCamera.target = position;
    ViewCamera.position = Vector3Add(ViewCamera.target, Vector3{ 0, 0, CameraPullbackDistance });
    ViewCamera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    ViewCamera.fovy = fovY;
    ViewCamera.projection = CAMERA_PERSPECTIVE;

    NearPlane = 0.01;
    FarPlane = 1000.0;

    ResizeTPOrbitCameraView(this);
    SetUseMouse(true, 1);
}

void rlTPCamera::SetUseMouse(bool useMouse, int button)
{
    UseMouse = useMouse;
    UseMouseButton = button;

    bool showCursor = !useMouse || button >= 0;

    if (!showCursor && IsWindowFocused())
        DisableCursor();
    else if (showCursor && IsWindowFocused())
        EnableCursor();
}

float rlTPCamera::GetSpeedForAxis(CameraControls axis, float speed)
{
    int key = ControlsKeys[axis];
    if (key == -1)
        return 0;

    float factor = 1.0f;
    if (IsKeyDown(ControlsKeys[SPRINT]))
        factor = 2;

    if (IsKeyDown(ControlsKeys[axis]))
        return speed * GetFrameTime() * factor;

    return 0.0f;
}

float rlTPCamera::GetFOVX() const
{
    return FOV.x;
}

Vector3 rlTPCamera::GetCameraPosition() const
{
    return CameraPosition;
}

void rlTPCamera::SetCameraPosition(const Vector3&& pos)
{
    CameraPosition = pos;
    Vector3 forward = Vector3Subtract(ViewCamera.target, ViewCamera.position);
    ViewCamera.position = CameraPosition;
    ViewCamera.target = Vector3Add(CameraPosition, forward);
}

Ray rlTPCamera::GetViewRay() const
{
    return Ray{ ViewCamera.position, GetForwardVector() };
}

Vector3 rlTPCamera::GetForwardVector() const
{
    return Vector3Normalize(Vector3Subtract(ViewCamera.target, ViewCamera.position));
}

Vector3 rlTPCamera::GetFowardGroundVector() const
{
    return ViewForward;
}

void rlTPCamera::Update()
{
    if (IsWindowResized())
        ResizeTPOrbitCameraView(this);

    bool showCursor = !UseMouse || UseMouseButton >= 0;

    if (IsWindowFocused() != Focused && !showCursor)
    {
        Focused = IsWindowFocused();
        if (Focused)
        {
            DisableCursor();
            PreviousMousePosition = GetMousePosition(); // so there is no jump on focus
        }
        else
        {
            EnableCursor();
        }
    }

    // Mouse movement detection
    Vector2 mousePositionDelta = GetMouseDelta();
    float mouseWheelMove = GetMouseWheelMove();

    // Keys input detection
    float direction[MOVE_DOWN + 1] = { -GetSpeedForAxis(CameraControls::MOVE_FRONT,MoveSpeed.z),
                                      -GetSpeedForAxis(CameraControls::MOVE_BACK,MoveSpeed.z),
                                      GetSpeedForAxis(CameraControls::MOVE_RIGHT,MoveSpeed.x),
                                      GetSpeedForAxis(CameraControls::MOVE_LEFT,MoveSpeed.x),
                                      GetSpeedForAxis(CameraControls::MOVE_UP,MoveSpeed.y),
                                      GetSpeedForAxis(CameraControls::MOVE_DOWN,MoveSpeed.y) };


    bool useMouse = UseMouse && (UseMouseButton < 0 || IsMouseButtonDown(UseMouseButton));

    float turnRotation = GetSpeedForAxis(CameraControls::TURN_RIGHT, TurnSpeed.x) - GetSpeedForAxis(CameraControls::TURN_LEFT, TurnSpeed.x);
    float tiltRotation = GetSpeedForAxis(CameraControls::TURN_UP, TurnSpeed.y) - GetSpeedForAxis(CameraControls::TURN_DOWN, TurnSpeed.y);

    if (turnRotation != 0)
        ViewAngles.x -= turnRotation * DEG2RAD;
    else if (useMouse && Focused)
        ViewAngles.x -= (mousePositionDelta.x / MouseSensitivity);

    if (tiltRotation)
        ViewAngles.y += tiltRotation * DEG2RAD;
    else if (useMouse && Focused)
        ViewAngles.y += (mousePositionDelta.y / -MouseSensitivity);

    // Angle clamp
    if (ViewAngles.y < MinimumViewY * DEG2RAD)
        ViewAngles.y = MinimumViewY * DEG2RAD;
    else if (ViewAngles.y > MaximumViewY * DEG2RAD)
        ViewAngles.y = MaximumViewY * DEG2RAD;

    //movement in plane rotation space
    Vector3 moveVec = { 0,0,0 };
    moveVec.z = direction[MOVE_FRONT] - direction[MOVE_BACK];
    moveVec.x = direction[MOVE_RIGHT] - direction[MOVE_LEFT];

    // update zoom
    CameraPullbackDistance += GetMouseWheelMove();
    if (CameraPullbackDistance < 1)
        CameraPullbackDistance = 1;

    // vector we are going to transform to get the camera offset from the target point
    Vector3 camPos = { 0, 0, CameraPullbackDistance };

    Matrix tiltMat = MatrixRotateX(ViewAngles.y); // a matrix for the tilt rotation
    Matrix rotMat = MatrixRotateY(ViewAngles.x); // a matrix for the plane rotation
    Matrix mat = MatrixMultiply(tiltMat, rotMat); // the combined transformation matrix for the camera position

    camPos = Vector3Transform(camPos, mat); // transform the camera position into a vector in world space
    moveVec = Vector3Transform(moveVec, rotMat); // transform the movement vector into world space, but ignore the tilt so it is in plane

    CameraPosition = Vector3Add(CameraPosition, moveVec); // move the target to the moved position

    // validate cam pos here

    // set the view camera
    ViewCamera.target = CameraPosition;
    ViewCamera.position = Vector3Add(CameraPosition, camPos); // offset the camera position by the vector from the target position
}

static void SetupCamera(rlTPCamera* camera, float aspect)
{
    rlDrawRenderBatchActive();			// Draw Buffers (Only OpenGL 3+ and ES2)
    rlMatrixMode(RL_PROJECTION);        // Switch to projection matrix
    rlPushMatrix();                     // Save previous matrix, which contains the settings for the 2d ortho projection
    rlLoadIdentity();                   // Reset current matrix (projection)

    if (camera->ViewCamera.projection == CAMERA_PERSPECTIVE)
    {
        // Setup perspective projection
        double top = RL_CULL_DISTANCE_NEAR * tan(camera->ViewCamera.fovy * 0.5 * DEG2RAD);
        double right = top * aspect;

        rlFrustum(-right, right, -top, top, camera->NearPlane, camera->FarPlane);
    }
    else if (camera->ViewCamera.projection == CAMERA_ORTHOGRAPHIC)
    {
        // Setup orthographic projection
        double top = camera->ViewCamera.fovy / 2.0;
        double right = top * aspect;

        rlOrtho(-right, right, -top, top, camera->NearPlane, camera->FarPlane);
    }

    // NOTE: zNear and zFar values are important when computing depth buffer values

    rlMatrixMode(RL_MODELVIEW);         // Switch back to modelview matrix
    rlLoadIdentity();                   // Reset current matrix (modelview)

    // Setup Camera view
    Matrix matView = MatrixLookAt(camera->ViewCamera.position, camera->ViewCamera.target, camera->ViewCamera.up);
    rlMultMatrixf(MatrixToFloat(matView));      // Multiply modelview matrix by view matrix (camera)

    rlEnableDepthTest();                // Enable DEPTH_TEST for 3D
}

// start drawing using the camera, with near/far plane support
void rlTPCamera::BeginMode3D()
{
    float aspect = (float)GetScreenWidth() / (float)GetScreenHeight();
    SetupCamera(this, aspect);
}

// end drawing with the camera
void rlTPCamera::EndMode3D()
{
    ::EndMode3D();
}