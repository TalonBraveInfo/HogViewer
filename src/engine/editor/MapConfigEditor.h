/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
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

#pragma once

#include "BaseWindow.h"

namespace ohw {
	class Map;
	class MapConfigEditor : public BaseWindow {
	public:
		MapConfigEditor();
		~MapConfigEditor() override;

		void Display() override;
		void SaveManifest( const std::string &path );
		void RestoreManifest();

	protected:
	private:
		MapManifest backup_;
		MapManifest *manifest_;
		Map *map_{ nullptr };

		char name_buffer[32]{ '\0' };
		char author_buffer[32]{ '\0' };
		char filename_buffer[32]{ '\0' };

		void DisplayWeather();
		void DisplayTemperature();
		void DisplayTime();
	};
}
