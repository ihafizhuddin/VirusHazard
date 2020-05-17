#include "Menu.h"


Menu::Menu()
{
}


Menu::~Menu()
{
	if (Mix_Playing(sfx_channel) == 0) {
		//Mix_FreeChunk(moveSound);
		//Mix_FreeChunk(gameClickSound);
		//Mix_FreeChunk(menuClickSound);
		//Mix_FreeChunk(matchSound);
		//Mix_FreeChunk(switchSound);
		//Mix_FreeChunk(sound);
	}
	if (music != NULL) {
		Mix_FreeMusic(music);
	}
	Mix_CloseAudio();
}


void Menu::Init()
{
	InitText();
	InitButton();
	this->program = BuildShader("shader.vert", "shader.frag");
	InputMapping("SelectButton", SDLK_RETURN);
	InputMapping("NextButton", SDLK_DOWN);
	InputMapping("PrevButton", SDLK_UP);
	InitAudio();
}

void Menu::DeInit() {

}

void Menu::Update(float deltaTime)
{
	if (Mix_Playing(sfx_channel) == 0 && IsKeyDown("SFX")) {
		//sfx_channel = Mix_PlayChannel(-1, sound, 0);
		if (sfx_channel == -1) {
			Err("Unable to play WAV file: " + string(Mix_GetError()));
		}
	}
	if (Mix_PlayingMusic() == 0)
	{
		//Play the music
		Mix_PlayMusic(music, -1);
		SDL_Delay(150);
	}
	//If music is being played
	else
	{
		//If the music is paused
		if (Mix_PausedMusic() == 1)
		{
			//Resume the music
			Mix_ResumeMusic();
			SDL_Delay(150);
		}
		//If the music is playing
		else
		{
			//Pause the music
			Mix_PauseMusic();
			SDL_Delay(150);
		}
	}
	if (IsKeyDown("SelectButton")) {
		if (activeButtonIndex == 0) {
			Mix_PlayMusic(menuClickSound, 0);
			
			//play();
		}
		else if (activeButtonIndex == 1) {
			Mix_PlayMusic(menuClickSound, 0);
		}
		else if (activeButtonIndex == 2) {
			Mix_PlayMusic(menuClickSound, 0);
		}
		else if (activeButtonIndex == 3) {
			Mix_PlayMusic(menuClickSound, 0);
		}
		else if (activeButtonIndex == 4) {
			Mix_PlayMusic(menuClickSound, 0);
			SDL_Quit();
			exit(0);
		}
	}

	if (IsKeyUp("NextButton")) {
		if (activeButtonIndex < NUM_BUTTON - 1) {
			Mix_PlayMusic(moveSound, 0);
			activeButtonIndex = activeButtonIndex + 1;
			SDL_Delay(150);
		}
	}

	if (IsKeyUp("PrevButton")) {
		if (activeButtonIndex > 0) {
			Mix_PlayMusic(moveSound, 0);
			activeButtonIndex = activeButtonIndex - 1;
			SDL_Delay(150);
		}
	}

}

void Menu::Render()
{
	//Setting Viewport
	glViewport(0, 0, GetScreenWidth(), GetScreenHeight());

	//Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set the background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program, "projection"), 1, GL_FALSE, value_ptr(projection));


	RenderText("Virus Hazard", GetScreenWidth()/2-150, 10, 1.0f, vec3(1, 0, 0));
	//RenderText("Virus Hazard", 10, 10, 1.0f, vec3(244.0f / 255.0f, 12.0f / 255.0f, 116.0f / 255.0f));

	RenderButton();

}

void Menu::InitText() {
	// Init Freetype
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		Err("ERROR::FREETYPE: Could not init FreeType Library");
	}
	FT_Face face;
	if (FT_New_Face(ft, FONTNAME, 0, &face)) {
		Err("ERROR::FREETYPE: Failed to load font");
	}

	FT_Set_Pixel_Sizes(face, 0, FONTSIZE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(pair<GLchar, Character>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, NULL,
		GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Menu::RenderText(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color)
{
	// Activate corresponding render state
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	UseShader(this->program);

	glUniform3f(glGetUniformLocation(this->program, "ourColor"), color.x, color.y, color.z);
	glUniform1i(glGetUniformLocation(this->program, "text"), 1);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(this->program, "ourTexture"), 0);
	mat4 model;
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));
	glBindVertexArray(VAO);

	// Iterate through all characters
	string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];
		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;
		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character

		GLfloat vertices[] = {
			// Positions   // Texture Coords
			xpos + w,	ypos + h,	1.0f, 1.0f, // Bottom Right
			xpos + w,	ypos,		1.0f, 0.0f, // Top Right
			xpos,		ypos,		0.0f, 0.0f, // Top Left
			xpos,		ypos + h,	0.0f, 1.0f  // Bottom Left 
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_QUADS, 0, 4);
		// Now advance cursors for next glyph (note that advance is number of 1 / 64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels(2 ^ 6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void Menu::InitButton() {
	string buttons[NUM_BUTTON] = { "button_play.png", "button_highscore.png", "button_options.png", "button_credits.png", "button_exit.png" };
	string hover_buttons[NUM_BUTTON] = { "button_play_hover.png", "button_highscore_hover.png",  "button_options_hover.png", "button_credits_hover.png", "button_exit_hover.png" };

	glGenTextures(5, &texture[0]);
	glGenTextures(5, &hover_texture[0]);

	for (int i = 0; i < NUM_BUTTON; i++) {
		// Load and create a texture 
		glBindTexture(GL_TEXTURE_2D, texture[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Load, create texture 
		int width, height;
		unsigned char* image = SOIL_load_image(buttons[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

		// Set up vertex data (and buffer(s)) and attribute pointers
		button_width[i] = (float)width;
		button_height[i] = (float)height;

		// Load and create a texture 
		glBindTexture(GL_TEXTURE_2D, hover_texture[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		image = SOIL_load_image(hover_buttons[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

		 // Set up vertex data (and buffer(s)) and attribute pointers
		hover_button_width[i] = (float)width;
		hover_button_height[i] = (float)height;
	}

	GLfloat vertices[] = {
		// Positions	// Texture Coords
		1,  1,			1.0f, 1.0f, // Bottom Right
		1,  0,			1.0f, 0.0f, // Top Right
		0,  0,			0.0f, 0.0f, // Top Left
		0,  1,			0.0f, 1.0f  // Bottom Left 
	};


	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Menu::RenderButton() {
	// Enable transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	int texture_id[NUM_BUTTON] = { GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3, GL_TEXTURE4, GL_TEXTURE5 };
	// Activate shader
	UseShader(this->program);
	glUniform1i(glGetUniformLocation(this->program, "text"), 0);

	glBindVertexArray(VAO2);
	for (int i = 0; i < NUM_BUTTON; i++) {

		glActiveTexture(texture_id[i]);
		glBindTexture(GL_TEXTURE_2D, (activeButtonIndex == i) ? hover_texture[i] : texture[i]);
		glUniform1i(glGetUniformLocation(this->program, "ourTexture"), i + 1);

		mat4 model;
		model = translate(model, vec3((GetScreenWidth() - button_width[i]) / 2, (i + 1) * 100, 0.0f));
		model = scale(model, vec3(button_width[i], button_height[i], 1));
		glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));

		glDrawArrays(GL_QUADS, 0, 4);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void Menu::InitAudio() {
	int flags = MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_OGG;
	if (flags != Mix_Init(flags)) {
		Err("Unable to initialize mixer: " + string(Mix_GetError()));
	}

	int audio_rate = 22050; Uint16 audio_format = AUDIO_S16SYS; int audio_channels = 2; int audio_buffers = 4096;

	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
		Err("Unable to initialize audio: " + string(Mix_GetError()));
	}


	menuClickSound = Mix_LoadMUS("click1.ogg");
	if (menuClickSound == NULL) {
		Err("Unable to load sound file: " + string(Mix_GetError()));
	}
	matchSound = Mix_LoadMUS("rollover1.ogg");
	if (matchSound== NULL) {
		Err("Unable to load WAV file: " + string(Mix_GetError()));
	}
	switchSound = Mix_LoadMUS("switch2.ogg");
	if (switchSound == NULL) {
		Err("Unable to load WAV file: " + string(Mix_GetError()));
	}
	moveSound = Mix_LoadMUS("switch3.ogg");
	if (moveSound == NULL) {
		Err("Unable to load WAV file: " + string(Mix_GetError()));
	}
	gameClickSound = Mix_LoadMUS("click2.ogg");
	if (gameClickSound == NULL) {
		Err("Unable to load WAV file: " + string(Mix_GetError()));
	}
	
	
}

int main(int argc, char** argv) {

	Engine::Game &game = Menu();
	game.Start("User Interface Menu", 800, 600, false, WindowFlag::WINDOWED, 60, 1);

	return 0;
}
void play() {
	//Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set the background color
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}
