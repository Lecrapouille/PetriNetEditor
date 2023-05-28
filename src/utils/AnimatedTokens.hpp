//=============================================================================
// TimedPetriNetEditor: A timed Petri net editor.
// Copyright 2021 -- 2023 Quentin Quadrat <lecrapouille@gmail.com>
//
// This file is part of TimedPetriNetEditor.
//
// TimedPetriNetEditor is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.
//=============================================================================

#ifndef ANIMATED_TOKENS_HPP
#  define ANIMATED_TOKENS_HPP

#  include "PetriNet.hpp"
#  include "utils/Utils.hpp"

// *****************************************************************************
//! \brief Tokens are systems resources. Places indicate how many tokens they
//! have but in this project, when simulation is run, we want to render them
//! moving along arcs Transitions -> Places (note there is no animation for arcs
//! Places -> Transitions: they are teleported). For the rendering, instead of
//! showing many tokens (dots) at the same position, we "group" them as a dot
//! with the number of tokens carried as caption. Since we are working on timed
//! petri nets arcs have a duration which is also constrain their velocity.
// *****************************************************************************
struct AnimatedToken
{
    //--------------------------------------------------------------------------
    //! \brief Constructor.
    //! \param[in] arc_: to which arc token are moving along. Shall be an arc
    //! Transition -> Place. No check is performed here.
    //! \param[in] tokens_: the number of tokens it shall carry.
    //! \param[in] type_: Type of the net (Petri, timed Petri, GRAFCET ...)
    //--------------------------------------------------------------------------
    AnimatedToken(Arc& arc_, size_t tokens_, PetriNet::Type type_)
        : arc(arc_), x(arc_.from.x), y(arc_.from.y), tokens(tokens_), type(type_)
    {
        assert(arc.from.type == Node::Type::Transition);
        assert(arc.to.type == Node::Type::Place);

        // Note: we are supposing the norm and duration is never updated by
        // the user during the simulation.
        if (type != PetriNet::Type::TimedGraphEvent)
        {
            magnitude = norm(arc.from.x, arc.from.y, arc.to.x, arc.to.y);
        }
        else
        {
            // With graph event we have to skip implicit places.
            assert(arc.to.arcsOut.size() == 1u && "malformed graph event");
            Node& next = arc.to.arcsOut[0]->to;
            magnitude = norm(arc.from.x, arc.from.y, next.x, next.y);
        }

        // Set the token animation speed. Depending on the type of Petri net,
        // and for pure entertainment reason, override the arc duration to
        // avoid unpleasant instaneous transitions (teleportation effect).
        switch (type_)
        {
        case PetriNet::Type::TimedPetri:
        case PetriNet::Type::TimedGraphEvent:
            speed = magnitude / std::max(0.000001f, arc.duration);
            break;
            // In theory duration is 0 but nicer for the user to see animation.
        case PetriNet::Type::Petri:
            speed = magnitude / 0.2f;
            break;
            // In theory duration is 0 but nicer for the user to see animation.
        case PetriNet::Type::GRAFCET:
            speed = magnitude / 1.5f;
            break;
        default: assert(false && "Unknown type of net"); break;
        }
    }

    // I dunno why the code in the #else branch seems to make buggy animations
    // with tokens that disapear. Cannot catch it by unit tests.
    // https://github.com/Lecrapouille/TimedPetriNetEditor/issues/2
# if 1

    //--------------------------------------------------------------------------
    //! \brief Hack needed because of references
    //--------------------------------------------------------------------------
    AnimatedToken& operator=(const AnimatedToken& obj)
    {
        this->~AnimatedToken(); // destroy
        new (this) AnimatedToken(obj); // copy construct in place
        return *this;
    }

    AnimatedToken(const AnimatedToken&) = default;
    AnimatedToken(AnimatedToken&&) = default;
    AnimatedToken& operator=(AnimatedToken&&) = default;

#else

    //--------------------------------------------------------------------------
    //! \brief Hack needed because of references
    //--------------------------------------------------------------------------
    AnimatedToken& operator=(AnimatedToken const& other)
    {
        this->~AnimatedToken(); // destroy
        new (this) AnimatedToken(other); // copy construct in place
        return *this;
    }

    //--------------------------------------------------------------------------
    //! \brief Needed to remove compilation warnings
    //--------------------------------------------------------------------------
    AnimatedToken(AnimatedToken const& other)
        : AnimatedToken(other.arc, other.tokens)
    {}

    //--------------------------------------------------------------------------
    //! \brief Needed to remove compilation warnings
    //--------------------------------------------------------------------------
    AnimatedToken(AnimatedToken&& other)
        : AnimatedToken(other.arc, other.tokens)
    {}

    //--------------------------------------------------------------------------
    //! \brief Needed to remove compilation warnings
    //--------------------------------------------------------------------------
    AnimatedToken& operator=(AnimatedToken&& other)
    {
        this->~AnimatedToken(); // destroy
        new (this) AnimatedToken(other); // copy construct in place
        return *this;
    }

#endif

    //--------------------------------------------------------------------------
    //! \brief Update position on the screen.
    //! \param[in] dt: the delta time (in seconds) from the previous call.
    //! \return true when arriving to the destination node (Place) else false.
    //--------------------------------------------------------------------------
    bool update(float const dt)
    {
        // With graph event we have to skip implicit places.
        Node& next = (type != PetriNet::Type::TimedGraphEvent)
                   ? arc.to : arc.to.arcsOut[0]->to;

        offset += dt * speed / magnitude;
        x = arc.from.x + (next.x - arc.from.x) * offset;
        y = arc.from.y + (next.y - arc.from.y) * offset;

        return (offset >= 1.0);
    }

    //--------------------------------------------------------------------------
    //! \brief Return the reference of the destination node casted as a Place.
    //! \note Since Tokens are animated from Transition to Places there is no
    //! confusion possible in the type of destination node.
    //--------------------------------------------------------------------------
    inline Place& toPlace()
    {
        return *reinterpret_cast<Place*>(&(arc.to));
    }

    //! \brief In which arc the token is moving along.
    Arc& arc;
    //! \brief X-axis coordinate in the window used for the display.
    float x;
    //! \brief Y-axis coordinate in the window used for the display.
    float y;
    //! \brief Number of carried tokens.
    size_t tokens;
    //! \brief
    PetriNet::Type type;
    //! \brief The length of the arc.
    float magnitude;
    //! \brief The speed of the token moving along the arc.
    float speed;
    //! \brief What ratio the token has transitioned over the arc (0%: origin
    //! position, 100%: destination position).
    float offset = 0.0f;
};

#endif
