/**********************************************************************************************
*
*   raylibExtras * Utilities and Shared Components for Raylib
*
*   RLAssets * Simple Asset Managment System for Raylib
*
*   LICENSE: MIT
*
*   Copyright (c) 2020 Jeffery Myers
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

#include "rlFPCamera.h"

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#include <cmath>

rlFPCamera::rlFPCamera() : ControlsKeys{ 'W', 'S', 'D', 'A', 'E', 'Q', KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_LEFT_SHIFT }
{

}

void rlFPCamera::Setup(float fovY, Vector3&& position)
{
    CameraPosition = position;
    ViewCamera.position = { position.x, position.y, position.z };
    ViewCamera.position.y += PlayerEyesPosition;
    ViewCamera.target = Vector3Add(ViewCamera.position, Vector3{ 0, 0, 1 });
    ViewCamera.up = { 0.0f, 1.0f, 0.0f };
    ViewCamera.fovy = fovY;
    ViewCamera.projection = CAMERA_PERSPECTIVE;

    TargetSize.x = GetScreenWidth();
    TargetSize.y = GetScreenHeight();

    Focused = IsWindowFocused();
    if (HideCursor && Focused && (UseMouseX || UseMouseY))
        DisableCursor();

    TargetDistance = 1;

    ViewResized();
}

void rlFPCamera::ViewResized()
{
    ViewResized((float)GetScreenWidth(), (float)GetScreenHeight());
}

void rlFPCamera::ViewResized(float w, float h)
{
    TargetSize.x = w;
    TargetSize.y = h;
    FOV.y = ViewCamera.fovy;

    if (h != 0)
        FOV.x = FOV.y * (w / h);
}

void rlFPCamera::ViewResized(const RenderTexture& target)
{
    ViewResized((float)target.texture.width, (float)target.texture.height);
}

float rlFPCamera::GetSpeedForAxis(CameraControls axis, float speed)
{
    if (!UseKeyboard)
        return 0;

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

void rlFPCamera::BeginMode3D()
{
    float aspect = TargetSize.x / TargetSize.y;

    rlDrawRenderBatchActive();			// Draw Buffers (Only OpenGL 3+ and ES2)
    rlMatrixMode(RL_PROJECTION);        // Switch to projection matrix
    rlPushMatrix();                     // Save previous matrix, which contains the settings for the 2d ortho projection
    rlLoadIdentity();                   // Reset current matrix (projection)

    if (ViewCamera.projection == CAMERA_PERSPECTIVE)
    {
        // Setup perspective projection
        double top = RL_CULL_DISTANCE_NEAR * tan(ViewCamera.fovy * 0.5 * DEG2RAD);
        double right = top * aspect;

        rlFrustum(-right, right, -top, top, NearPlane, FarPlane);
    }
    else if (ViewCamera.projection == CAMERA_ORTHOGRAPHIC)
    {
        // Setup orthographic projection
        double top = ViewCamera.fovy / 2.0;
        double right = top * aspect;

        rlOrtho(-right, right, -top, top, NearPlane, FarPlane);
    }

    // NOTE: zNear and zFar values are important when computing depth buffer values

    rlMatrixMode(RL_MODELVIEW);         // Switch back to modelview matrix
    rlLoadIdentity();                   // Reset current matrix (modelview)

    // Setup Camera view
    Matrix matView = MatrixLookAt(ViewCamera.position, ViewCamera.target, ViewCamera.up);
    rlMultMatrixf(MatrixToFloat(matView));      // Multiply modelview matrix by view matrix (camera)

    rlEnableDepthTest();                // Enable DEPTH_TEST for 3D
}

void rlFPCamera::EndMode3D()
{
    ::EndMode3D();
}

Ray rlFPCamera::GetViewRay() const
{
    return Ray{ViewCamera.position, GetForwardVector() };
}

Vector3 rlFPCamera::GetForwardVector() const
{
    return Vector3Normalize(Vector3Subtract(ViewCamera.target, ViewCamera.position));
}

Vector3 rlFPCamera::GetFowardGroundVector() const
{
    Vector3 forward = Vector3Subtract(ViewCamera.target, ViewCamera.position);
    forward.y = 0;
    forward = Vector3Normalize(forward);

    return forward;
}

void rlFPCamera::Update()
{
    if (HideCursor && IsWindowFocused() != Focused && (UseMouseX || UseMouseY))
    {
        Focused = IsWindowFocused();
        if (Focused)
        {
            DisableCursor();
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
    float direction[MOVE_DOWN + 1] = { GetSpeedForAxis(MOVE_FRONT,MoveSpeed.z),
                                      GetSpeedForAxis(MOVE_BACK,MoveSpeed.z),
                                      GetSpeedForAxis(MOVE_RIGHT,MoveSpeed.x),
                                      GetSpeedForAxis(MOVE_LEFT,MoveSpeed.x),
                                      GetSpeedForAxis(MOVE_UP,MoveSpeed.y),
                                      GetSpeedForAxis(MOVE_DOWN,MoveSpeed.y) };

    if (UseController && IsGamepadAvailable(ControlerID))
    {
        bool sprint = IsKeyDown(ControlsKeys[SPRINT]) || IsGamepadButtonDown(ControlerID, ControlerSprintButton);
        float factor = MoveSpeed.z * GetFrameTime();
        if (sprint)
            factor *= 2;

        float forward = -GetGamepadAxisMovement(ControlerID, ControlerForwardAxis) * factor;
        if (forward > 0)
        {
            direction[MOVE_FRONT] = std::max(direction[MOVE_FRONT], forward);
            direction[MOVE_BACK] = 0;
        }
        else if(forward < 0)
        {
            direction[MOVE_BACK] = std::max(direction[MOVE_BACK], fabs(forward));
            direction[MOVE_FRONT] = 0;
        }

        factor = MoveSpeed.x * GetFrameTime();
        if (sprint)
            factor *= 2;

        float side = GetGamepadAxisMovement(ControlerID, ControllerSideAxis) * factor;
        if (side > 0)
        {
            direction[MOVE_RIGHT] = std::max(direction[MOVE_RIGHT], side);
            direction[MOVE_LEFT] = 0;
        }
        else if (side < 0)
        {
            direction[MOVE_LEFT] = std::max(direction[MOVE_LEFT], fabs(side));
            direction[MOVE_RIGHT] = 0;
        }
    }

    Vector3 forward = Vector3Subtract(ViewCamera.target, ViewCamera.position);
    forward.y = 0;
    forward = Vector3Normalize(forward);

    Vector3 right{ forward.z * -1.0f, 0, forward.x };

    Vector3 oldPosition = CameraPosition;

    CameraPosition = Vector3Add(CameraPosition, Vector3Scale(forward, direction[MOVE_FRONT] - direction[MOVE_BACK]));
    CameraPosition = Vector3Add(CameraPosition, Vector3Scale(right, direction[MOVE_RIGHT] - direction[MOVE_LEFT]));

    CameraPosition.y += direction[MOVE_UP] - direction[MOVE_DOWN];

    // let someone modify the projected position
    if (ValidateCamPosition != nullptr)
        ValidateCamPosition(*this, CameraPosition, oldPosition);

    // Camera orientation calculation
    float turnRotation = GetSpeedForAxis(TURN_LEFT, TurnSpeed.x) - GetSpeedForAxis(TURN_RIGHT, TurnSpeed.x);
    float tiltRotation = GetSpeedForAxis(TURN_DOWN, TurnSpeed.y) - GetSpeedForAxis(TURN_UP, TurnSpeed.y);

    if (UseController && IsGamepadAvailable(ControlerID))
    {
        float factor = GetFrameTime() * TurnSpeed.x;

        float turn = GetGamepadAxisMovement(ControlerID, ControllerYawAxis) * factor;
        if (turn > 0)
        {
            turnRotation = std::max(turnRotation, turn);
        }
        else if (turn < 0)
        {
            turnRotation = std::min(turnRotation, turn);
        }

        factor = GetFrameTime() * TurnSpeed.y;
        float tilt = -GetGamepadAxisMovement(ControlerID, ControllerPitchAxis) * factor;
        if (tilt > 0)
        {
            tiltRotation = std::max(turnRotation, tilt);
        }
        else if (tilt < 0)
        {
            tiltRotation = std::min(turnRotation, tilt);
        }
    }

    float yFactor = InvertY ? -1.0f : 1.0f;

    if (turnRotation != 0)
        Angle.x -= turnRotation * DEG2RAD;
    else if (UseMouseX && Focused)
        Angle.x += (mousePositionDelta.x / MouseSensitivity);

    if (tiltRotation)
        Angle.y += yFactor * tiltRotation * DEG2RAD;
    else if (UseMouseY && Focused)
        Angle.y += (yFactor * mousePositionDelta.y / MouseSensitivity);

    // Angle clamp
    if (Angle.y < MinimumViewY * DEG2RAD)
        Angle.y = MinimumViewY * DEG2RAD;
    else if (Angle.y > MaximumViewY * DEG2RAD)
        Angle.y = MaximumViewY * DEG2RAD;

    // Recalculate camera target considering translation and rotation
    Vector3 target = Vector3Transform(Vector3{ 0, 0, 1 }, MatrixRotateZYX(Vector3{ Angle.y, -Angle.x, 0 }));

    ViewCamera.position = CameraPosition;

    float eyeOfset = PlayerEyesPosition;

    if (ViewBobbleFreq > 0)
    {
        float swingDelta = std::fmax(std::abs(direction[MOVE_FRONT] - direction[MOVE_BACK]), std::abs(direction[MOVE_RIGHT] - direction[MOVE_LEFT]));

        // If movement detected (some key pressed), increase swinging
        CurrentBobble += swingDelta * ViewBobbleFreq;

        constexpr float ViewBobbleDampen = 8.0f;

        eyeOfset -= sinf(CurrentBobble / ViewBobbleDampen) * ViewBobbleMagnatude;

        ViewCamera.up.x = sinf(CurrentBobble / (ViewBobbleDampen * 2)) * ViewBobbleWaverMagnitude;
        ViewCamera.up.z = -sinf(CurrentBobble / (ViewBobbleDampen * 2)) * ViewBobbleWaverMagnitude;
    }
    else
    {
        CurrentBobble = 0;
        ViewCamera.up.x = 0;
        ViewCamera.up.z = 0;
    }

    ViewCamera.position.y += eyeOfset;

    ViewCamera.target.x = ViewCamera.position.x + target.x;
    ViewCamera.target.y = ViewCamera.position.y + target.y;
    ViewCamera.target.z = ViewCamera.position.z + target.z;
}

float rlFPCamera::GetFOVX() const
{
    return FOV.x;
}

Vector3 rlFPCamera::GetCameraPosition() const
{
    return CameraPosition;
}

void rlFPCamera::SetCameraPosition(const Vector3&& pos)
{ 
    CameraPosition = pos;
    Vector3 forward = Vector3Subtract(ViewCamera.target, ViewCamera.position);
    ViewCamera.position = CameraPosition;
    ViewCamera.target = Vector3Add(CameraPosition, forward);
}

Ray rlFPCamera::GetMouseRay(Vector2 mouse) const
{
    Ray ray = { 0 };

    // Calculate normalized device coordinates
    // NOTE: y value is negative
    float x = (2.0f * mouse.x) / TargetSize.x - 1.0f;
    float y = 1.0f - (2.0f * mouse.y) / (float)TargetSize.y;
    float z = 1.0f;

    // Store values in a vector
    Vector3 deviceCoords = { x, y, z };

    // Calculate view matrix from camera look at
    Matrix matView = MatrixLookAt(ViewCamera.position, ViewCamera.target, ViewCamera.up);

    Matrix matProj = MatrixIdentity();

    if (ViewCamera.projection == CAMERA_PERSPECTIVE)
    {
        // Calculate projection matrix from perspective
        matProj = MatrixPerspective(ViewCamera.fovy * DEG2RAD, ((double)TargetSize.x / (double)TargetSize.y), RL_CULL_DISTANCE_NEAR, RL_CULL_DISTANCE_FAR);
    }
    else if (ViewCamera.projection == CAMERA_ORTHOGRAPHIC)
    {
        float aspect = TargetSize.x / TargetSize.y;
        double top = ViewCamera.fovy / 2.0;
        double right = top * aspect;

        // Calculate projection matrix from orthographic
        matProj = MatrixOrtho(-right, right, -top, top, 0.01, 1000.0);
    }

    // Unproject far/near points
    Vector3 nearPoint = Vector3Unproject(Vector3{ deviceCoords.x, deviceCoords.y, 0.0f }, matProj, matView);
    Vector3 farPoint = Vector3Unproject(Vector3{ deviceCoords.x, deviceCoords.y, 1.0f }, matProj, matView);

    // Unproject the mouse cursor in the near plane.
    // We need this as the source position because orthographic projects, compared to perspect doesn't have a
    // convergence point, meaning that the "eye" of the camera is more like a plane than a point.
    Vector3 cameraPlanePointerPos = Vector3Unproject(Vector3{ deviceCoords.x, deviceCoords.y, -1.0f }, matProj, matView);

    // Calculate normalized direction vector
    Vector3 direction = Vector3Normalize(Vector3Subtract(farPoint, nearPoint));

    if (ViewCamera.projection == CAMERA_PERSPECTIVE)
        ray.position = ViewCamera.position;
    else if (ViewCamera.projection == CAMERA_ORTHOGRAPHIC) 
        ray.position = cameraPlanePointerPos;

    // Apply calculated vectors to ray
    ray.direction = direction;

    return ray;
}