/* OpenHoW
 * Copyright (C) 2017-2020 Mark Sowden <markelswo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "base_window.h"

class Camera;

class ModelViewer : public BaseWindow {
public:
	explicit ModelViewer();
	~ModelViewer() override;

	void Display() override;

	void DrawViewport();

protected:
private:
	struct PLModel *modelPtr{ nullptr };
	struct PLFrameBuffer *drawBuffer{ nullptr };
	struct PLTexture *textureAttachment{ nullptr };

	Camera *camera{ nullptr };

	PLVector3 modelRotation;

	float oldMousePos[2]{ 0, 0 };

	bool viewFullscreen{ false };
	bool viewRotate{ true };
	bool viewDebugNormals{ false };

	static void AppendModelList( const char *path );
	static std::list<std::string> modelList;
};
