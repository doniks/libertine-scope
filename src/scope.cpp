/*
 * Copyright 2015-2016 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "scope.h"

#include <iostream>
#include <localization.h>
#include "preview.h"
#include "query.h"
#include <sstream>
#include <unity/scopes/ActivationResponse.h>
#include <url-dispatcher.h>


namespace usc = unity::scopes;

namespace
{

/**
 * @todo move this class into its own source file.
 */
class ScopeActivation
: public unity::scopes::ActivationQueryBase
{
public:
  ScopeActivation(usc::Result const& result,
                  usc::ActionMetadata const& metadata)
  : ActivationQueryBase(result, metadata)
  { }

  unity::scopes::ActivationResponse
  activate() override
  {
    return unity::scopes::ActivationResponse(status);
  }

  unity::scopes::ActivationResponse::Status status = unity::scopes::ActivationResponse::Status::NotHandled;
};

} // anonymous namespace


void Scope::
start(std::string const&)
{
  std::cerr << "==smw> " << __PRETTY_FUNCTION__ << "() begins\n";
  setlocale(LC_ALL, "");
  try{
  std::string translation_directory = ScopeBase::scope_directory()
                                    + "/../share/locale/";
  bindtextdomain(GETTEXT_PACKAGE, translation_directory.c_str());
  } catch (std::exception& ex) {
    std::cerr << "==smw> " << __PRETTY_FUNCTION__ << "() exception caught: " << ex.what() << "\n";
  }
  std::cerr << "==smw> " << __PRETTY_FUNCTION__ << "() ends\n";
}


void Scope::
stop()
{
}


usc::SearchQueryBase::UPtr Scope::
search(usc::CannedQuery const&    query,
       usc::SearchMetadata const& metadata)
{
  return usc::SearchQueryBase::UPtr(new Query(query, metadata));
}


usc::PreviewQueryBase::UPtr Scope::
preview(usc::Result const&         result,
        usc::ActionMetadata const& metadata)
{
  return usc::PreviewQueryBase::UPtr(new Preview(result, metadata));
}


usc::ActivationQueryBase::UPtr Scope::
perform_action(usc::Result const&         result,
               usc::ActionMetadata const& metadata,
               std::string const&         /* widget_id */,
               std::string const&         action_id)
{
  auto activation = new ScopeActivation(result, metadata);

  if (action_id == "open")
  {
    url_dispatch_send(result.uri().c_str() , NULL, NULL);
  }

  return usc::ActivationQueryBase::UPtr(activation);
}


#define EXPORT __attribute__((visibility ("default")))

// These functions define the entry points for the scope plugin
extern "C" {

EXPORT unity::scopes::ScopeBase*
// cppcheck-suppress unusedFunction
UNITY_SCOPE_CREATE_FUNCTION()
{
  return new Scope();
}


EXPORT void
// cppcheck-suppress unusedFunction
UNITY_SCOPE_DESTROY_FUNCTION(unity::scopes::ScopeBase* scope_base)
{
  delete scope_base;
}

}

