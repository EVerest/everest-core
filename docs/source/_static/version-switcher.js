document.addEventListener("DOMContentLoaded", function() {
    const versionsJson = "/versions.json"; 
    
    const basePath = "/"; 

    fetch(versionsJson)
        .then(response => response.json())
        .then(data => {
            // 1. Get array of versions
            const versions = data.versions; 

            // 2. Build the UI elements
            const container = document.createElement("div");
            container.className = "version-switcher-container";
            container.style.padding = "1rem";
            container.style.backgroundColor = "#24ce7b";

            // 2a Label
            const label = document.createElement("label");
            label.textContent = "Version";
            label.style.display = "block";
            label.style.marginBottom = "0.3rem";
            label.style.fontSize = "0.75rem";
            label.style.fontWeight = "bold";
            label.style.color = "var(--color-foreground-secondary)";
            label.htmlFor = "version-switcher-select";
            container.appendChild(label);

            // 2b Selector dropdown
            const select = document.createElement("select");
            select.id = "version-switcher-select";
            select.style.width = "100%";
            select.style.padding = "0.3rem";
            select.style.backgroundColor = "var(--color-background-secondary)";
            select.style.color = "var(--color-foreground-primary)";
            select.style.border = "1px solid var(--color-background-border)";
            select.style.borderRadius = "0.2rem";

            const currentPath = window.location.pathname;

            // 2c Populate dropdown menu
            versions.forEach(version_string => {
                const option = document.createElement("option");
                
                // url and name
                option.value = `${basePath}${version_string}/`; 
                option.textContent = version_string; 
                
                // Mark as selected if the current URL contains this version string
                if (currentPath.includes(`/${version_string}/`)) {
                    option.selected = true;
                }
                select.appendChild(option);
            });

            // Extract the sub-path after the current version prefix, e.g.
            // "/v2.0/tutorials/install.html" -> "tutorials/install.html"
            function currentSubPath() {
                const match = currentPath.match(/^\/[^/]+\/(.+)$/);
                return match ? match[1] : null;
            }

            select.addEventListener("change", function() {
                const versionRoot = this.value;
                const subPath = currentSubPath();
                if (subPath) {
                    const candidate = versionRoot + subPath;
                    fetch(candidate, { method: "HEAD" })
                        .then(response => {
                            window.location.href = response.ok ? candidate : versionRoot;
                        })
                        .catch(() => { window.location.href = versionRoot; });
                } else {
                    window.location.href = versionRoot;
                }
            });

            container.appendChild(select);

            // 4. function which modifies the sidebar with the version switcher
            function injectSidebar() {
                const sidebar = document.querySelector(".sidebar-container");
                if (sidebar && !document.querySelector(".version-switcher-container")) {
                    sidebar.insertBefore(container, sidebar.firstChild);
                    return true;
                }
                return false;
            }

            // 5. Try to inject it, or wait for the DOM to be ready
            if (!injectSidebar()) {
                // Wait for Trailbook to finish building the DOM
                const observer = new MutationObserver((mutations, obs) => {
                    if (injectSidebar()) {
                        obs.disconnect(); 
                    }
                });
                observer.observe(document.body, { childList: true, subtree: true });
            }
        })
        .catch(error => console.error("Error loading version list:", error));
});
