//
// Game.cpp
//

#include "pch.h"
#include "Game.h"


//toreorganise
#include <fstream>

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace ImGui;

using Microsoft::WRL::ComPtr;
float t = 0;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
#ifdef DXTK_AUDIO
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{

	m_input.Initialise(window);

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

	//setup imgui.  its up here cos we need the window handle too
	//pulled from imgui directx11 example
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(window);		//tie to our window
	ImGui_ImplDX11_Init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());	//tie to directx

	m_fullscreenRect.left = 0;
	m_fullscreenRect.top = 0;
	m_fullscreenRect.right = 800;
	m_fullscreenRect.bottom = 600;

	m_CameraViewRect.left = 500;
	m_CameraViewRect.top = 0;
	m_CameraViewRect.right = 800;
	m_CameraViewRect.bottom = 240;

    m_bloomRect.left = 0;
    m_bloomRect.top = 0;
    m_bloomRect.right = m_fullscreenRect.right / 2;
    m_bloomRect.bottom = m_fullscreenRect.bottom / 2;

	//setup light
	m_Light.setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	m_Light.setDiffuseColour(1.0f, 0.5f, 0.0f, 0.3f);
	m_Light.setPosition(5.0f, 4.0f, 20.0f);
	m_Light.setDirection(-1.0f, -1.0f, 0.0f);

	//setup camera
	m_Camera01.setPosition(Vector3(20.0f, 5.0f, 20.0f));
	m_Camera01.setRotation(Vector3(0.0f, -180.0f, 0.0f));	//orientation is -90 becuase zero will be looking up at the sky straight up. 
    m_Camera02.setPosition(Vector3(15.0f, 10.0f, 10.0f));
	m_Camera02.setRotation(Vector3(-90.0f, -180.0f, 0.0f));	//orientation is -90 becuase zero will be looking up at the sky straight up. 
    
    current_camera = 0;

    m_bloomEffect.g_BloomPresets = { 0,  0,   0, 1,    0,       1 };
    game = false;
    water_a = 0.25;
    speed = 3;
#ifdef DXTK_AUDIO
    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audioEvent = 0;
    m_audioTimerAcc = 10.f;
    m_retryDefault = false;

    m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"MusicMono_adpcm.wav");
    m_effect1 = m_soundEffect->CreateInstance();
    m_effect2 = m_waveBank->CreateInstance(10);

    m_effect1->Play(true);
    m_effect2->Play();
#endif
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	//take in input
	m_input.Update();								//update the hardware
	m_gameInputCommands = m_input.getGameInput();	//retrieve the input for our game
	
	//Update all game objects
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

	//Render all game content. 
    Render();

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif

	
}

float t2 = 0;
// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{	
	//this is hacky,  i dont like this here.  
	auto device = m_deviceResources->GetD3DDevice();

    double delta_time = timer.GetElapsedSeconds();
    delta_time *= 100.0;

	//note that currently.  Delta-time is not considered in the game object movement. 
	if (m_gameInputCommands.left)
	{
        
        if (game == 1) {

            Vector3 center = hook.GetCenter() + Vector3(-0.1, 0, 0);
            if(!(center.x<0))
                hook.SetCenter(center);

        }
        else {
            Vector3 rotation = m_Camera01.getRotation();
            rotation.y = rotation.y += m_Camera01.getRotationSpeed() * delta_time;
            m_Camera01.setRotation(rotation);

        }
	}
	if (m_gameInputCommands.right)
	{
        
        if (game ) {
            Vector3 center = hook.GetCenter() + Vector3(0.1, 0, 0);
            if (!(center.x > 32))
                hook.SetCenter(center);
        }
        else {
            Vector3 rotation = m_Camera01.getRotation();
            rotation.y = rotation.y -= m_Camera01.getRotationSpeed() * delta_time;
            m_Camera01.setRotation(rotation);

        }
	}
	if (m_gameInputCommands.forward)
	{
        

        if (game ) {

            Vector3 center = hook.GetCenter() + Vector3(0, 0, -0.1);
            if (!(center.z < 0))
                hook.SetCenter(center);

        }
        else {
            Vector3 position = m_Camera01.getPosition(); //get the position
            position += (m_Camera01.getForward() * m_Camera01.getMoveSpeed()) * delta_time; //add the forward vector
            m_Camera01.setPosition(position);
        }
	}
	if (m_gameInputCommands.back)
	{
        

        if (game) {

            Vector3 center = hook.GetCenter() + Vector3(0, 0, 0.1);
            if (!(center.z > 11))
                hook.SetCenter(center);

        }
        else {
            Vector3 position = m_Camera01.getPosition(); //get the position
            position -= (m_Camera01.getForward() * m_Camera01.getMoveSpeed()) * delta_time; //add the forward vector
            m_Camera01.setPosition(position);
        }
	}

    if (m_gameInputCommands.smooth) {
        m_Terrain.Smoothing(device);
    }
    if (m_Terrain.activateMidPoint == true) {
 
        m_Terrain.ResetTerrain(device, 128, 128);
        m_Terrain.GenerateMidPoint(128, m_Terrain.m_range, true, device);
        
    }
    if (m_gameInputCommands.lclick && game) {

        /*float pointX, pointY;
        auto width = m_deviceResources->GetScreenViewport().Width;
        auto height = m_deviceResources->GetScreenViewport().Height;
        Vector3 direction;

        // Move the mouse cursor coordinates into the -1 to +1 range.
        pointX = ((2.0f * (float)m_gameInputCommands.x_pos) / (float)width) - 1.0f;
        pointY = (((2.0f * (float)m_gameInputCommands.y_pos) / (float)height) - 1.0f) * -1.0f;

        // Adjust the points using the projection matrix to account for the aspect ratio of the viewport.
        pointX = pointX / m_projection._11;
        pointY = pointY / m_projection._22;

        // Get the inverse of the view matrix.
        m_view = m_Camera01.getCameraMatrix();
        Matrix m_inverse_view = m_view.Invert();

        // Calculate the direction of the picking ray in view space.
        direction.x = (pointX * m_inverse_view._11) + (pointY * m_inverse_view._21) + m_inverse_view._31;
        direction.y = (pointX * m_inverse_view._12) + (pointY * m_inverse_view._22) + m_inverse_view._32;
        direction.z = (pointX * m_inverse_view._13) + (pointY * m_inverse_view._23) + m_inverse_view._33;
        direction.z = -direction.z;
        // Get the world matrix and translate to the location of the sphere.
        m_world = Matrix::Identity; //set world back to identity
        Matrix newPosition = SimpleMath::Matrix::CreateTranslation(sphere.GetCenter().x, sphere.GetCenter().y, sphere.GetCenter().z);
        m_world = m_world * newPosition;
        
        // Now get the inverse of the translated world matrix.
        Matrix m_inverse_world = m_world.Invert();

        // Now transform the ray origin and the ray direction from view space to world space.
        Vector3 rayOrigin, rayDirection;
        m_Camera01.getPosition().Transform(m_Camera01.getPosition(), m_inverse_world,rayOrigin);
        direction.TransformNormal(direction, m_inverse_world,rayDirection);
        
        // Normalize the ray direction.
        rayDirection.Normalize();*/

        Vector3 rayOrigin = hook.GetCenter();
        Vector3 rayDirection = Vector3(0, -1, 0);

        // Now perform the ray-sphere intersection test.
        bool collision = sphere.SphereCollision(rayOrigin, rayDirection);
        if (collision) {
            sphere.render = false;

        }

    } 
    if (m_gameInputCommands.rclick) {

        
        Vector3 delta = Vector3(float(m_gameInputCommands.x_pos), float(m_gameInputCommands.y_pos), 0.f) ;
        m_gameInputCommands.m_pitch -= delta.y;
        m_gameInputCommands.m_yaw -= delta.x;
        
        float limit = XM_PI / 2.0f - 0.01f;
        m_gameInputCommands.m_pitch = std::max(-limit, m_gameInputCommands.m_pitch);
        m_gameInputCommands.m_pitch = std::min(+limit, m_gameInputCommands.m_pitch);

        if (m_gameInputCommands.m_yaw > XM_PI)
        {
            m_gameInputCommands.m_yaw -= XM_PI * 2.0f;
        }
        else if (m_gameInputCommands.m_yaw < -XM_PI)
        {
            m_gameInputCommands.m_yaw += XM_PI * 2.0f;
        }
        //if (m_gameInputCommands.m_pitch > XM_PI)
            m_gameInputCommands.m_pitch += XM_PI * 2.0f;
           // m_gameInputCommands.m_pitch += XM_PI * 2.0f;


        m_Camera01.setRotation(Vector3(0, m_gameInputCommands.m_yaw, 0));
        

    }


  
	    m_Camera01.Update();	//camera update.
    
	    m_Camera02.Update();	//camera update.

	m_Terrain.Update();		//terrain update.  doesnt do anything at the moment. 

    if(!game)
	    m_view = m_Camera01.getCameraMatrix();
    else {
        m_view = m_Camera02.getCameraMatrix();

        m_Terrain.m_range = 7;

        t2 += delta_time;
        if (t2>(120*speed)) {

            float x = rand() % 31;
            float z = rand() % 9;

            Vector3 v = Vector3(x, 1, z);
            sphere.SetCenter(v);
            t2 = 0;
        }
    }

	m_world = Matrix::Identity;

	/*create our UI*/
	SetupGUI();

#ifdef DXTK_AUDIO
    m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                // Restart looping audio
                m_effect1->Play(true);
            }
        }
        else
        {
            m_audioTimerAcc = 4.f;

            m_waveBank->Play(m_audioEvent++);

            if (m_audioEvent >= 11)
                m_audioEvent = 0;
        }
    }
#endif

  
	if (m_input.Quit())
	{
		ExitGame();
	}
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{	
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }
    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTargetView = m_deviceResources->GetRenderTargetView();
    auto depthTargetView = m_deviceResources->GetDepthStencilView();

    RenderFirstPass();
    PostProcess();

    m_sprites->Begin();
    m_sprites->Draw(m_CombinePass->getShaderResourceView(), m_fullscreenRect); //display render to texture
    m_sprites->End();

    // Draw Text to the screen
    m_sprites->Begin();
    m_font->DrawString(m_sprites.get(), L"Procedural Methods", XMFLOAT2(10, 10), Colors::Yellow);
    m_sprites->End();


    if (game && !sphere.render) {

        // Draw Text to the screen
        m_sprites->Begin();
        m_font->DrawString(m_sprites.get(), L"Good Catch!", XMFLOAT2(20,50), Colors::Red);
        m_sprites->End();
    }

    //render our GUI
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    
    m_deviceResources->Present();
}

void Game::RenderScene(ID3D11DeviceContext* context)
{
   

    //Set Rendering states. 
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
    context->RSSetState(m_states->CullClockwise());
    //	context->RSSetState(m_states->Wireframe());

    Matrix proj;
    if (!game )
        proj = m_projection;
    else
        proj = m_projection_ortho;


    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix boxpos = SimpleMath::Matrix::CreateTranslation(0.0f, 50.0f, 0.0f);
    //SimpleMath::Matrix newScale = SimpleMath::Matrix::CreateScale(0.25);		//scale the terrain down a little. 
    m_world = m_world * boxpos;

    context->OMSetDepthStencilState(m_states->DepthNone(), 0);

    m_SkyShader.EnableShader(context);
    m_SkyShader.SetShaderSkyBoxParameters(context,m_Camera01.getPosition(), &m_world, &m_view, &proj, m_skybox.Get());
    m_BasicModel3.Render(context);

    context->OMSetDepthStencilState(m_states->DepthDefault(), 0);

    //prepare transform for floor object. 
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(0.0f, 3.0f, 0.0f);
    SimpleMath::Matrix newScale = SimpleMath::Matrix::CreateScale(0.25);		//scale the terrain down a little. 
    m_world = m_world * newScale * newPosition;

    //setup and draw cube
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &proj, &m_Light, m_texture1.Get());
    m_Terrain.Render(context);

    //prepare transform for floor object. 
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPosition2 = SimpleMath::Matrix::CreateTranslation(0.0f, 1.0f, -0.0f);
    SimpleMath::Matrix newScale2 = SimpleMath::Matrix::CreateScale(0.25);		//scale the terrain down a little. 
    m_world = m_world * newScale2 * newPosition2;

    context->OMSetBlendState(m_states->AlphaBlend(), nullptr, 0xFFFFFFFF);

    //setup and draw cube
    t+=m_timer.GetElapsedSeconds();
    m_PerlinShader.EnableShader(context);
    m_PerlinShader.SetPerlinParameters(context, &m_world, &m_view, &proj,t,water_a, &m_Light, m_water_tex.Get());
    m_water.Render(context);

    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);

    //prepare transform for floor object. 
    //sphere.SetRad(0.1);
   
    if (sphere.render) {
        m_world = SimpleMath::Matrix::Identity; //set world back to identity
        m_world = m_world * SimpleMath::Matrix::CreateScale(sphere.GetRad()) * SimpleMath::Matrix::CreateTranslation(sphere.GetCenter().x, sphere.GetCenter().y, sphere.GetCenter().z);
        //setup and draw sphere
        m_BasicShaderPair.EnableShader(context);
        m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &proj, &m_Light, m_texture2.Get());
        sphere.ball.Render(context);
    }
    if (hook.render) {

        m_world = SimpleMath::Matrix::Identity; //set world back to identity
        m_world = m_world * SimpleMath::Matrix::CreateScale(hook.GetRad()) * SimpleMath::Matrix::CreateTranslation(hook.GetCenter().x, hook.GetCenter().y, hook.GetCenter().z);

        //setup and draw sphere
        m_BasicShaderPair.EnableShader(context);
        m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &proj, &m_Light, m_texture3.Get());
        hook.ball.Render(context);
    }

}

void Game::RenderFirstPass()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTargetView = m_deviceResources->GetRenderTargetView();
    auto depthTargetView = m_deviceResources->GetDepthStencilView();
    // Set the render target to be the render to texture.

    m_FirstRenderPass->setRenderTarget(context);
    // Clear the render to texture.
    m_FirstRenderPass->clearRenderTarget(context, 0.0f, 0.0f, 1.0f, 1.0f);

    RenderScene(context);

    // Reset the render target back to the original back buffer and not the render to texture anymore.	
    context->OMSetRenderTargets(1, &renderTargetView, depthTargetView);
}

void Game::PostProcess()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTargetView = m_deviceResources->GetRenderTargetView();
    auto depthTargetView = m_deviceResources->GetDepthStencilView();

    //***********************Bloom Extract***********************//
    m_BloomExtractPass->setRenderTarget(context);

    // Clear the render to texture.
    m_BloomExtractPass->clearRenderTarget(context, 0.0f, 0.0f, 1.0f, 1.0f);

    m_BloomExtractShader.EnableShader(context);
    m_BloomExtractShader.SetBlomExtractParameters(context, m_bloomEffect.g_BloomPresets.bloomThreshold, m_FirstRenderPass->getShaderResourceView());

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
    context->Draw(4, 0);//quad
    context->OMSetRenderTargets(1, &renderTargetView, depthTargetView);

    //***********************Gaussian Blur***********************//

    //HORIZONTAL
    m_GausianBlurPass->setRenderTarget(context);

    // Clear the render to texture.
    m_GausianBlurPass->clearRenderTarget(context, 0.0f, 0.0f, 0.0f, 0.0f);

    m_GaussianBlurShader.EnableShader(context);
    m_GaussianBlurShader.SetGaussianBlurParameters(context, m_blurEffect.sampleOffsets, m_blurEffect.sampleWeights, m_blurEffect.SAMPLE_COUNT, m_BloomExtractPass->getShaderResourceView());

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
    context->Draw(4, 0);//quad
    context->OMSetRenderTargets(1, &renderTargetView, depthTargetView);

    //VERTICAL

    m_GausianBlurPass2->setRenderTarget(context);

    // Clear the render to texture.
    m_GausianBlurPass2->clearRenderTarget(context, 0.0f, 0.0f, 0.0f, 0.0f);

    m_GaussianBlurShader.EnableShader(context);
    m_GaussianBlurShader.SetGaussianBlurParameters(context, m_blurEffect.sampleOffsets, m_blurEffect.sampleWeights, m_blurEffect.SAMPLE_COUNT, m_GausianBlurPass->getShaderResourceView());

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
    context->Draw(4, 0);//quad
    context->OMSetRenderTargets(1, &renderTargetView, depthTargetView);

    //***********************Final result***********************//

    m_CombinePass->setRenderTarget(context);

    // Clear the render to texture.
    m_CombinePass->clearRenderTarget(context, 0.0f, 0.0f, 0.0f, 0.0f);

    m_CombineShader.EnableShader(context);
    m_CombineShader.SetCombineParameters(context,m_bloomEffect.g_BloomPresets.bloomSaturation, m_bloomEffect.g_BloomPresets.bloomIntensity, m_bloomEffect.g_BloomPresets.baseSaturation, m_bloomEffect.g_BloomPresets.baseIntensity,m_FirstRenderPass->getShaderResourceView(),m_GausianBlurPass2->getShaderResourceView());

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
    context->Draw(4, 0);//quad
    context->OMSetRenderTargets(1, &renderTargetView, depthTargetView);
}


// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);
    m_deviceResources->PIXEndEvent();
}

#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef DXTK_AUDIO
    m_audEngine->Suspend();
#endif
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

#ifdef DXTK_AUDIO
    m_audEngine->Resume();
#endif
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);

    m_fullscreenRect.right = r.right;
    m_fullscreenRect.bottom = r.bottom;
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;
   
    m_fullscreenRect.right = width;
    m_fullscreenRect.bottom = height;
    CreateWindowSizeDependentResources();
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
}
#endif

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    m_states = std::make_unique<CommonStates>(device);
    m_fxFactory = std::make_unique<EffectFactory>(device);
    m_sprites = std::make_unique<SpriteBatch>(context);
    m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	//setup our terrain
	m_Terrain.Initialize(device, 128, 128);
	m_water.Initialize(device, 128, 128);

    sphere.ball.InitializeSphere(device);
    sphere.SetCenter(Vector3(20, 1, 10));

    hook.ball.InitializeSphere(device);
    hook.SetRad(0.5);
    hook.SetCenter(Vector3(5, 5, 0));

    auto size = m_deviceResources->GetOutputSize();
    m_blurEffect.SetBlurEffectParameters(1.f / (size.right), 0, m_bloomEffect.g_BloomPresets);
    m_blurEffect2.SetBlurEffectParameters( 0,1.f / (size.bottom) , m_bloomEffect.g_BloomPresets);
        

	m_BasicModel3.InitializeSkyBox(device, 100.0f, 100.0f, 100.0f);	//box includes dimensions

	//load and set up our Vertex and Pixel Shaders
	m_BasicShaderPair.InitStandard(device, L"light_vs.cso", L"light_ps.cso");
	m_SkyShader.InitSkyBox(device, L"skybox_vs.cso", L"skybox_ps.cso");

	m_BloomExtractShader.InitBloomExtract(device, L"quad_vs.cso", L"BloomExtract.cso");
	m_GaussianBlurShader.InitGaussianBlur(device, L"quad_vs.cso", L"GaussianBlur.cso");
	m_CombineShader.InitBloomCombine(device, L"quad_vs.cso", L"BloomCombine.cso");
	m_PerlinShader.InitPerlinNoise(device, L"light_vs.cso", L"pnoise_ps.cso");

	//load Textures
	CreateDDSTextureFromFile(device, L"mountain.dds",		nullptr,	m_texture1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"orange.dds", nullptr,	m_texture2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"red.dds", nullptr,	m_texture3.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"skybox3.dds", nullptr,	m_skybox.ReleaseAndGetAddressOf(), D3D11_RESOURCE_MISC_TEXTURECUBE);
    CreateDDSTextureFromFile(device, L"water.dds", nullptr,	m_water_tex.ReleaseAndGetAddressOf(), D3D11_RESOURCE_MISC_TEXTURECUBE);
    
    
	//Initialise Render to texture
	m_FirstRenderPass = new RenderTexture(device, 800, 600, 1, 2);	//for our rendering, We dont use the last two properties. but.  they cant be zero and they cant be the same. 
	m_BloomExtractPass = new RenderTexture(device, 800, 600, 1, 2);	//for our rendering, We dont use the last two properties. but.  they cant be zero and they cant be the same. 
	m_GausianBlurPass = new RenderTexture(device, 800, 600, 1, 2);	//for our rendering, We dont use the last two properties. but.  they cant be zero and they cant be the same. 
	m_GausianBlurPass2 = new RenderTexture(device, 800, 600, 1, 2);	//for our rendering, We dont use the last two properties. but.  they cant be zero and they cant be the same. 
	m_CombinePass = new RenderTexture(device, 800, 600, 1, 2);	//for our rendering, We dont use the last two properties. but.  they cant be zero and they cant be the same. 

    m_Terrain.m_range = 7;
    m_Terrain.GenerateMidPoint(128, m_Terrain.m_range, true, device);

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    m_projection = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        1000.0f
    );
    m_projection_ortho = Matrix::CreateOrthographic(size.right/20, size.bottom/20, -11.0f, 11.0f);

   
}

void Game::SetupGUI()
{

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("CMP505");
		//ImGui::SliderFloat("Wave Amplitude",	m_Terrain.GetAmplitude(), 0.0f, 10.0f);
		//ImGui::SliderFloat("Wavelength",		m_Terrain.GetWavelength(), 0.0f, 1.0f);
    if (ImGui::CollapsingHeader("Mid-Point Algorithm")) {
        ImGui::SliderInt("Randomness Range", m_Terrain.GetRange(), 0.0, 10.0);
        if (ImGui::Button("GENERATE"))
            m_Terrain.activateMidPoint = true;
    }
    if (ImGui::CollapsingHeader("Bloom Effect")) {

        if (ImGui::Button("None"))
            m_bloomEffect.g_BloomPresets = { 0,  0,   0, 1,    0,       1 };
        if (ImGui::Button("Default"))
            m_bloomEffect.g_BloomPresets = { 0.25f,  4,   1.25f, 1,    1,       1 };
        if (ImGui::Button("Soft"))
            m_bloomEffect.g_BloomPresets = { 0,      3,   1,     1,    1,       1 };
        if (ImGui::Button("Desaturated"))
            m_bloomEffect.g_BloomPresets = { 0.5f,   8,   2,     1,    0,       1 };
        if (ImGui::Button("Saturated"))
            m_bloomEffect.g_BloomPresets = { 0.25f,  4,   2,     1,    2,       0 };
        if (ImGui::Button("Blurry"))
            m_bloomEffect.g_BloomPresets = { 0,      2,   1,     0.1f, 1,       1 };
        if (ImGui::Button("Subtle"))
            m_bloomEffect.g_BloomPresets = { 0.5f,   2,   1,     1,    1,       1 };

    }
    if (ImGui::CollapsingHeader("Game")) {

        ImGui::SliderFloat("Ball Speed", &speed, 10, 0.1);

        if (ImGui::Button("Start")) {
            current_camera = 1;
            sphere.render = true;
            hook.render = true;
            game = true;
        }
        if (ImGui::Button("End")) {
            current_camera = 0;
            sphere.render = false;
            hook.render = false;
            game = false;

        }

    }
    if (ImGui::CollapsingHeader("General")) {
       
        ImGui::SliderFloat("Cam Rotation", m_Camera01.getRotationX(), -90, 90);
        ImGui::SliderFloat("Water Transparency", &water_a, 0, 1);

            
    }

	ImGui::End();
    
}


void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_font.reset();
	m_batch.reset();
	m_testmodel.reset();
    m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}
#pragma endregion
