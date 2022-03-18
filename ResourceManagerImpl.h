/*
 * This file is part of the CitizenFX project - http://citizen.re/
 *
 * See LICENSE and MENTIONS in the root of the source tree for information
 * regarding licensing.
 */

#pragma once

#include <ResourceImpl.h>
#include <ResourceManager.h>

#include <mutex>
#include <unordered_map>

namespace fx
{
class ResourceManagerImpl final : public ResourceManager
{
private:
	std::recursive_mutex m_resourcesMutex;

	std::unordered_map<std::string, fwRefContainer<ResourceImpl>> m_resources;

	std::unordered_multimap<std::string, fwRefContainer<Resource>> m_resourceProvides;

	std::recursive_mutex m_mountersMutex;

	std::vector<fwRefContainer<ResourceMounter>> m_mounters;

public:
	ResourceManagerImpl();

	void AddResourceInternal(fwRefContainer<Resource> resource);

	virtual void MakeCurrent() override;

	virtual fwRefContainer<ResourceMounter> GetMounterForUri(const std::string& uri) override;

	virtual pplx::task<fwRefContainer<Resource>> AddResource(const std::string& uri) override;

	//virtual pplx::task<tl::expected<fwRefContainer<Resource>, ResourceManagerError>> AddResourceWithError(const std::string& uri) override;

	virtual fwRefContainer<Resource> GetResource(const std::string& identifier, bool withProvides) override;

	virtual void ForAllResources(const std::function<void(const fwRefContainer<Resource>&)>& function) override;

	virtual void ResetResources() override;

	virtual void AddMounter(fwRefContainer<ResourceMounter> mounter) override;

	virtual void RemoveResource(fwRefContainer<Resource> resource) override;

	virtual fwRefContainer<Resource> CreateResource(const std::string& resourceName, const fwRefContainer<ResourceMounter>& mounter) override;

	virtual void Tick() override;

	virtual std::string CallReferenceInternal(const std::string& functionReference, const std::string& argsSerialized) override;
};
}
