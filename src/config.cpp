#include "config.hpp"
#include <algorithm>

namespace mkweb
{
namespace
{
template <class Container>
static bool in(const typename Container::value_type & element, const Container & container)
{
	return std::find(begin(container), end(container), element) != end(container);
}
}

config::config(const std::string & filename) { node_ = YAML::LoadFile(filename); }

std::string config::get_source() const { return get_str("source", "pages"); }

std::string config::get_destination() const { return get_str("destination", "pages"); }

std::string config::get_static() const { return get_str("static", "pages"); }

std::string config::get_plugin_path(const std::string & plugin) const
{
	auto path = get_str("plugins", {});
	if (path.size()) {
		path += "/" + plugin + "/";
	}
	return path;
}

std::string config::get_site_url() const { return get_str("site_url", "?"); }

std::string config::get_plugin_url() const { return get_str("plugin_url", ""); }

std::string config::get_site_title() const { return get_str("site_title", "TITLE"); }

std::string config::get_site_subtitle() const { return get_str("site_subtitle", ""); }

std::string config::get_language() const { return get_str("language", ""); }

std::string config::get_theme() const { return get_str("theme", "default"); }

std::string config::get_author() const { return get_str("author", "?"); }

int config::get_num_news() const { return get_int("num_news", 8); }

std::vector<std::string> config::get_source_process_filetypes() const
{
	const auto & types = node_["source-process-filetypes"];

	std::vector<std::string> result;
	if (types && types.IsSequence()) {
		result.reserve(types.size());
		for (const auto & type : types) {
			result.push_back(type.as<std::string>());
		}
	} else {
		result.push_back(".md");
	}
	return result;
}

std::vector<config::path_map_entry> config::get_path_map() const
{
	const auto & pmap = node_["path_map"];

	std::vector<path_map_entry> entries;
	if (pmap && pmap.IsSequence()) {
		entries.reserve(pmap.size());
		for (const auto & entry : pmap) {
			path_map_entry t;
			if (entry["base"])
				t.base = entry["base"].as<std::string>();
			if (entry["url"])
				t.url = entry["url"].as<std::string>();
			if (entry["absolute"])
				t.absolute = entry["absolute"].as<bool>();
			entries.push_back(t);
		}
	}
	return entries;
}

bool config::get_social_enable() const { return get_bool("social-enable", false); }

std::string config::get_social() const { return get_str("social", ""); }

bool config::get_menu_enable() const { return get_bool("menu-enable", false); }

std::string config::get_menu() const { return get_str("menu", ""); }

bool config::get_tags_enable() const { return get_bool("tags-enable", false); }

bool config::get_page_tags_enable() const { return get_bool("page-tags-enable", false); }

bool config::get_years_enable() const { return get_bool("years-enable", false); }

bool config::get_pagelist_enable() const { return get_bool("pagelist-enable", false); }

config::pagelist_sort_desc config::get_pagelist_sort() const
{
	static const std::vector<std::string> valid_directions = {"ascending", "descending"};
	static const std::vector<std::string> valid_keys = {"title", "date"};

	const auto & pls = node_["pagelist-sort"];

	pagelist_sort_desc result{sort_direction::ascending, "title"};

	if (pls) {
		if (pls["direction"]) {
			const auto s = pls["direction"].as<std::string>();
			if (in(s, valid_directions)) {
				if (s == "descending") {
					result.dir = sort_direction::descending;
				}
			}
		}
		if (pls["key"]) {
			const auto s = pls["key"].as<std::string>();
			if (in(s, valid_keys)) {
				result.key = s;
			}
		}
	}
	return result;
}

std::string config::get_site_title_background() const
{
	return get_themed("site_title_background");
}

std::string config::get_copyright() const { return get_themed("copyright"); }

std::string config::get_str(const std::string & tag, const std::string & default_value) const
{
	return (node_[tag] && node_[tag].IsScalar()) ? node_[tag].as<std::string>() : default_value;
}

int config::get_int(const std::string & tag, int default_value) const
{
	return (node_[tag] && node_[tag].IsScalar()) ? node_[tag].as<int>() : default_value;
}

bool config::get_bool(const std::string & tag, bool default_value) const
{
	return (node_[tag] && node_[tag].IsScalar()) ? node_[tag].as<bool>() : default_value;
}

std::string config::get_themed(const std::string field, const std::string & default_value) const
{
	const auto & tc = node_["theme-config"];
	if (!tc)
		return default_value;

	return (tc[field] && tc[field].IsScalar()) ? tc[field].as<std::string>() : default_value;
}
}
