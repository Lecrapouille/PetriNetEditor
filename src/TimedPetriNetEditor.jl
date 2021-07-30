##=====================================================================
## TimedPetriNetEditor: A timed Petri net editor.
## Copyright 2021 Quentin Quadrat <lecrapouille@gmail.com>
##
## This file is part of TimedPetriNetEditor.
##
## PetriEditor is free software: you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.
##=====================================================================

# Julia code calling the entry point C++ funtion of the timed Petri net
# editor (tpne). The C++ project shall be installed first.
ccall((:entry_point, "libtpne.so"), Cint, ())
