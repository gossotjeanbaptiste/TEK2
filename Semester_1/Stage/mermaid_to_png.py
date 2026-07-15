#!/usr/bin/env python3
"""
Script pour convertir les diagrammes Mermaid en PNG
Utilise Mermaid.js directement avec Playwright
"""

import asyncio
import sys
import re
from pathlib import Path
from playwright.async_api import async_playwright


async def mermaid_to_png(mermaid_code: str, output_path: str, index: int):
    """
    Convertit du code Mermaid en PNG en utilisant Mermaid.js directement
    """
    async with async_playwright() as p:
        browser = await p.chromium.launch()
        page = await browser.new_page()
        
        try:
            # Créer une page HTML avec Mermaid.js intégré
            html_content = f"""
            <!DOCTYPE html>
            <html>
            <head>
                <meta charset="UTF-8">
                <script src="https://cdn.jsdelivr.net/npm/mermaid/dist/mermaid.min.js"></script>
                <style>
                    * {{ 
                        margin: 0; 
                        padding: 0; 
                        box-sizing: border-box;
                    }}
                    html, body {{ 
                        background: white;
                        margin: 0;
                        padding: 0;
                        display: block;
                    }}
                    #diagram-container {{ 
                        background: white;
                        display: block;
                        padding: 0;
                        margin: 0;
                    }}
                    .mermaid {{
                        display: block !important;
                        margin: 0 !important;
                        padding: 0 !important;
                    }}
                </style>
            </head>
            <body>
                <div id="diagram-container">
                    <div class="mermaid">
{mermaid_code}
                    </div>
                </div>
                <script>
                    mermaid.initialize({{ startOnLoad: true, theme: 'default' }});
                    mermaid.contentLoaded();
                </script>
            </body>
            </html>
            """
            
            await page.set_content(html_content)
            
            # Attendre que Mermaid charge et rende le SVG
            await page.wait_for_selector('svg', timeout=15000)
            await page.wait_for_timeout(3000)
            
            # Récupérer les dimensions réelles du SVG rendu
            dimensions = await page.evaluate("""
                () => {
                    const svg = document.querySelector('svg');
                    if (svg) {
                        // Utiliser les dimensions exactes du SVG
                        const rect = svg.getBoundingClientRect();
                        return {
                            width: Math.ceil(rect.width + 20),
                            height: Math.ceil(rect.height + 20)
                        };
                    }
                    return { width: 800, height: 600 };
                }
            """)
            
            print(f"  Dimensions calculées: {dimensions['width']}x{dimensions['height']}")
            
            # Redimensionner le viewport aux dimensions réelles
            await page.set_viewport_size({"width": dimensions['width'], "height": dimensions['height']})
            await page.wait_for_timeout(2000)
            
            # Capturer la page complète
            await page.screenshot(path=output_path, full_page=True)
            print(f"✅ PNG créé: {output_path}")
                
        except Exception as e:
            print(f"❌ Erreur: {e}")
        finally:
            await browser.close()


def extract_mermaid_blocks(markdown_file: str):
    """
    Extrait tous les blocs de code Mermaid d'un fichier Markdown
    """
    with open(markdown_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Pattern pour les blocs mermaid
    pattern = r'```mermaid\n(.*?)\n```'
    blocks = re.findall(pattern, content, re.DOTALL)
    
    return blocks


async def main():
    md_file = "FLUX_DONNEES_CMS_GITHUB.md"
    
    if not Path(md_file).exists():
        print(f"❌ Erreur: {md_file} introuvable")
        sys.exit(1)
    
    print(f"📖 Lecture de {md_file}...")
    blocks = extract_mermaid_blocks(md_file)
    
    if not blocks:
        print("❌ Aucun diagramme Mermaid trouvé")
        sys.exit(1)
    
    print(f"📊 {len(blocks)} diagrammes trouvés\n")
    
    # Créer dossier de sortie
    output_dir = Path("diagrams_png")
    output_dir.mkdir(exist_ok=True)
    
    # Convertir chaque diagramme
    for i, block in enumerate(blocks, 1):
        output_path = output_dir / f"diagram_{i:02d}.png"
        print(f"⏳ Conversion diagramme {i}/{len(blocks)}...")
        
        try:
            await mermaid_to_png(block, str(output_path), i)
        except Exception as e:
            print(f"❌ Erreur pour diagramme {i}: {e}")
    
    print(f"\n✅ Terminé! PNG sauvegardés dans: {output_dir}")


if __name__ == "__main__":
    asyncio.run(main())
