/*
   This file is part of darktable,
   copyright (c) 2015 Jeremy Rosen

   darktable is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   darktable is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with darktable.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "lua/widget/widget.h"
#include "lua/types.h"

typedef struct {
  dt_lua_widget_t parent;
} dt_lua_radio_button_t;

typedef dt_lua_radio_button_t* lua_radio_button;

static void radio_button_init(lua_State* L);
static dt_lua_widget_type_t radio_button_type = {
  .name = "radio_button",
  .gui_init = radio_button_init,
  .gui_cleanup = NULL,
};

static void clicked_callback(GtkButton *widget, gpointer user_data)
{
  dt_lua_widget_trigger_callback_async((lua_widget)user_data,"clicked");
}

static void radio_button_init(lua_State* L)
{
  const char * new_value = NULL;
  lua_settop(L,2);
  if(!lua_isnil(L,1)){
    new_value = luaL_checkstring(L,1);
  }
  lua_radio_button radio_button = malloc(sizeof(dt_lua_radio_button_t));
  if(new_value) {
    radio_button->parent.widget = gtk_radio_button_new_with_label(NULL,new_value);
  } else {
    radio_button->parent.widget = gtk_radio_button_new(NULL);
  }


  radio_button->parent.type = &radio_button_type;
  luaA_push_type(L, radio_button_type.associated_type, &radio_button);
  g_object_ref_sink(radio_button->parent.widget);

  if(!lua_isnil(L,2)){
    lua_pushvalue(L,2);
    dt_lua_widget_set_callback(L,-2,"clicked");
  }
}


static int group_with(lua_State* L)
{
  lua_radio_button first;
  luaA_to(L,lua_radio_button,&first,1);
  lua_radio_button second = NULL;
  if(!lua_isnil(L,2)) {
    luaA_to(L,lua_radio_button,&second,2);
    gtk_radio_button_join_group(GTK_RADIO_BUTTON(first->parent.widget),GTK_RADIO_BUTTON(second->parent.widget));
  } else {
    gtk_radio_button_join_group(GTK_RADIO_BUTTON(first->parent.widget),NULL);
  }
  return 0;
}

static int label_member(lua_State *L)
{
  lua_radio_button radio_button;
  luaA_to(L,lua_radio_button,&radio_button,1);
  if(lua_gettop(L) > 2) {
    const char * label = luaL_checkstring(L,3);
    gtk_button_set_label(GTK_BUTTON(radio_button->parent.widget),label);
    return 0;
  }
  lua_pushstring(L,gtk_button_get_label(GTK_BUTTON(radio_button->parent.widget)));
  return 1;
}

static int value_member(lua_State *L)
{
  lua_radio_button radio_button;
  luaA_to(L,lua_radio_button,&radio_button,1);
  if(lua_gettop(L) > 2) {
    luaL_checktype(L,3,LUA_TBOOLEAN);
    gboolean value = lua_toboolean(L,3);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button->parent.widget),value);
    return 0;
  }
  lua_pushboolean(L,gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_button->parent.widget)));
  return 1;
}

int dt_lua_init_widget_radio_button(lua_State* L)
{
  dt_lua_init_widget_type(L,&radio_button_type,lua_radio_button);

  lua_pushcfunction(L,label_member);
  lua_pushcclosure(L,dt_lua_gtk_wrap,1);
  dt_lua_type_register(L, lua_radio_button, "label");
  lua_pushcfunction(L,value_member);
  lua_pushcclosure(L,dt_lua_gtk_wrap,1);
  dt_lua_type_register(L, lua_radio_button, "value");

  dt_lua_widget_register_gtk_callback(L,lua_radio_button,"clicked","clicked_callback",G_CALLBACK(clicked_callback));

  lua_pushcfunction(L,group_with);
  lua_pushcclosure(L,dt_lua_gtk_wrap,1);
  lua_pushcclosure(L, dt_lua_type_member_common, 1);
  dt_lua_type_register_const(L,lua_radio_button,"group_with");


  return 0;
}
// modelines: These editor modelines have been set for all relevant files by tools/update_modelines.sh
// vim: shiftwidth=2 expandtab tabstop=2 cindent
// kate: tab-indents: off; indent-width 2; replace-tabs on; indent-mode cstyle; remove-trailing-space on;
