import graphviz
import re
from pathlib import Path

# --- Arborescence de fichiers ---
file_tree_text = """
.
├── Comment_regler_les_soucis_du_CMS.docx
├── README.md
├── app.py
├── compile.bat
├── dependecies.bat
├── file_tree_colored.png
├── graph.py
├── index.html
├── robots.txt
├── run.bat
├── setup_git_safe.bat
├── sitemap.xml
├── cms
│   ├── CMS.md
│   ├── DOCUMENTATION.md
│   ├── README_CMS.md
│   ├── requirements.txt
│   ├── icon
│   │   ├── logo-routart.png
│   │   └── logo_routart.ico
│   └── source
│       ├── __init__.py
│       ├── build_exe.py
│       ├── config_manager.py
│       ├── git_manager.py
│       ├── html_manager.py
│       ├── logger.py
│       ├── rout_art_cms.py
│       ├── server_manager.py
│       ├── sitemap_generator.py
│       └── rout_art_cms.spec
├── dist
│   └── Rout_Art_CMS.exe
├── files
│   ├── reglement_interieur.docx
│   ├── moto
│   │   ├── 125cm3
│   │   │   └── programme_formation_125cm3.pdf
│   │   ├── a1
│   │   │   ├── parcours_formation_a1.pdf
│   │   │   └── programme_formation_a1.pdf
│   │   ├── a2
│   │   │   ├── enjeux_formation_a2.pdf
│   │   │   ├── parcours_formation_a2.pdf
│   │   │   └── programme_formation_a2.pdf
│   │   ├── a2_a
│   │   │   └── parcours_formation_a.pdf
│   │   └── am
│   │   │   ├── enjeux_formation_permis_am.pdf
│   │   │   └── programme_formation_permis_am.pdf
│   └── voiture
│       ├── enjeux_formation_permis_b.pdf
│       ├── parcours_formation_b.pdf
│       ├── programme_formation_b.pdf
│       └── auto
│           └── programme_auto_manuel.pdf
├── icon
│   └── logo_routart_modern.png
├── images
│   ├── france-travail-sombre.png
│   ├── france-travail.png
│   ├── garantie-financiere.png
│   ├── hero-car-road.png
│   ├── icon-car.png
│   ├── icon-facebook.ico
│   ├── icon-instagram.png
│   ├── icon-moto.png
│   ├── logo_routart_modern.png
│   ├── permis-1e.png
│   ├── prepa_code.png
│   ├── avatar
│   │   ├── avatar-emma.png
│   │   └── avatar-paul.png
│   ├── moto
│   │   ├── am-bsr.png
│   │   ├── permis-a1-125.png
│   │   └── permis-a2.png
│   ├── remerciement
│   │   ├── avatar_jean-baptiste.png
│   │   ├── github_ico.svg
│   │   ├── github_ico_darkmode.svg
│   │   ├── linkedin_ico.png
│   │   ├── portfolio_ico.png
│   │   ├── portfolio_ico_darkmode.png
│   │   └── techno
│   │   │   ├── github_ico.svg
│   │   │   ├── github_ico_dark.svg
│   │   │   ├── html.png
│   │   │   ├── html_dark.png
│   │   │   └── python.png
│   └── voiture
│       ├── permis-b-accompagnee.png
│       ├── permis-b-automatique.png
│       └── permis-b-traditionnel.png
├── page
│   ├── code_de_la_route.html
│   ├── contact.html
│   ├── equipe.html
│   ├── financement.html
│   ├── formations.html
│   ├── garantie_financiere.html
│   ├── positionnement.html
│   ├── presse.html
│   ├── reclamation.html
│   ├── reglement.html
│   ├── remerciement.html
│   ├── resultat.html
│   ├── satisfaction.html
│   ├── tarifs.html
│   └── template.html
├── script
│   ├── dark-mode.js
│   ├── financement.js
│   ├── formations.js
│   ├── remerciement.js
│   ├── script.js
│   └── tarifs.js
└── style
    ├── README.md
    ├── code_de_la_route.css
    ├── contact.css
    ├── dark-mode.css
    ├── equipe.css
    ├── financement.css
    ├── formations.css
    ├── garantie_financiere.css
    ├── general.css
    ├── index.css
    ├── positionnement.css
    ├── presse.css
    ├── reclamation.css
    ├── reglement.css
    ├── remerciement.css
    ├── resultat.css
    ├── satisfaction.css
    ├── tarifs.css
    └── variables.css
"""

# --- Configuration des Couleurs ---
FOLDER_COLOR = {'fill': '#FFC107', 'font': '#000000', 'border': '#C49000'}
ROOT_COLOR = {'fill': '#3F51B5', 'font': '#000000'}

# Mapping des extensions vers des couleurs
FILE_COLORS = {
    '.py': {'fill': '#306998', 'font': '#000000'},
    '.html': {'fill': '#E44D26', 'font': '#000000'},
    '.css': {'fill': '#264DE4', 'font': '#000000'},
    '.js': {'fill': '#F7DF1E', 'font': '#000000'},
    '.pdf': {'fill': '#B71C1C', 'font': '#000000'},
    '.docx': {'fill': '#1976D2', 'font': '#000000'},
    '.md': {'fill': '#757575', 'font': '#000000'},
    '.txt': {'fill': '#9E9E9E', 'font': '#000000'},
    '.png': {'fill': '#8E24AA', 'font': '#000000'},
    '.ico': {'fill': '#8E24AA', 'font': '#000000'},
    '.svg': {'fill': '#8E24AA', 'font': '#000000'},
    '.exe': {'fill': '#00796B', 'font': '#000000'},
    '.bat': {'fill': '#00796B', 'font': '#000000'},
    '.xml': {'fill': '#FB8C00', 'font': '#000000'},
    '.spec': {'fill': '#546E7A', 'font': '#000000'},
    'default': {'fill': '#CFD8DC', 'font': '#000000'}
}

# --- Libellés pour la légende ---
LEGEND_LABELS = {
    'Dossier': FOLDER_COLOR,
    'Python (.py)': FILE_COLORS['.py'],
    'Web (HTML)': FILE_COLORS['.html'],
    'Style (CSS)': FILE_COLORS['.css'],
    'Script (JS)': FILE_COLORS['.js'],
    'Document PDF': FILE_COLORS['.pdf'],
    'Word (.docx)': FILE_COLORS['.docx'],
    'Markdown (.md)': FILE_COLORS['.md'],
    'Image (.png/svg)': FILE_COLORS['.png'],
    'Exécutable (.exe/.bat)': FILE_COLORS['.exe'],
    'Config (.xml/.spec)': FILE_COLORS['.xml'],
    'Fichier texte': FILE_COLORS['.txt']
}


def parse_tree_text(tree_text):
    lines = tree_text.strip().split('\n')
    if lines and 'Rout-Art tree' in lines[0]:
        lines = lines[2:]

    paths = []
    indent_map = {}

    for line in lines:
        clean_line = re.sub(r'[\s\t\n\r│├─└──]+', '', line).strip()
        if not clean_line:
            continue

        match = re.search(r'[a-zA-Z0-9._-]+', line)
        if not match:
            continue

        name = match.group(0)
        indent_index = match.start()

        parent_path = ''
        if indent_index > 0:
            parent_indent = max(
                [i for i in indent_map if i < indent_index] or [-1])
            if parent_indent != -1:
                parent_path = indent_map.get(parent_indent, '')

        current_path = Path(parent_path) / name
        paths.append(str(current_path))
        indent_map[indent_index] = str(current_path)

    return paths


def get_file_style(filename):
    ext = Path(filename).suffix.lower()
    style = FILE_COLORS.get(ext, FILE_COLORS['default'])
    return style['fill'], style['font']


def add_legend_node(dot):
    """Crée un nœud HTML complexe servant de légende - Version GROSSE TAILLE."""

    html_label = '<<TABLE BORDER="0" CELLBORDER="0" CELLSPACING="8" CELLPADDING="4">'
    # Titre en taille 24
    html_label += '<TR><TD COLSPAN="2"><FONT POINT-SIZE="24"><B>Légende des types</B></FONT></TD></TR>'
    html_label += '<HR/>'

    for label, style in LEGEND_LABELS.items():
        fill = style['fill']

        # Augmentation de la taille des carrés de couleur et de la police (16)
        html_label += f'''
        <TR>
            <TD BGCOLOR="{fill}" WIDTH="50" HEIGHT="30" BORDER="1" FIXEDSIZE="TRUE"></TD>
            <TD ALIGN="LEFT"><FONT FACE="Arial" POINT-SIZE="16">{label}</FONT></TD>
        </TR>
        '''

    html_label += '</TABLE>>'

    dot.node('Legend', label=html_label, shape='plain')


def create_file_tree_graph(file_tree_text, output_filename="file_tree_print"):
    paths = parse_tree_text(file_tree_text)

    dot = graphviz.Digraph(
        'FileTree',
        format='png',
        graph_attr={
            'rankdir': 'LR',
            'splines': 'polyline',
            'ranksep': '2.5',     # Plus d'espace horizontal pour les gros textes
            'nodesep': '0.15',    # Plus d'espace vertical entre les fichiers
            'bgcolor': "#EBDFDF",
            'ordering': 'out',
            'concentrate': 'true',
            'dpi': '300'
        },
        node_attr={
            'shape': 'rect',
            'style': 'filled,rounded',
            'fontname': 'Consolas',
            'fontsize': '20',     # <--- AUGMENTATION TAILLE POLICE (était 10)
            # <--- AUGMENTATION HAUTEUR BOITE (était 0.25)
            'height': '0.4',
            'margin': '0.15',
            'penwidth': '0'
        },
        edge_attr={
            'color': "#464F53",
            'arrowhead': 'none',
            'penwidth': '1.5'     # Trait un peu plus épais pour l'impression
        }
    )

    root_name = 'Rout-Art'

    # 1. Création de la légende
    add_legend_node(dot)

    # 2. Création de la racine (Taille 24)
    dot.node(root_name, label=root_name,
             fillcolor=ROOT_COLOR['fill'], fontcolor=ROOT_COLOR['font'],
             fontname='Arial-Bold', fontsize='24', height='0.6')

    # 3. Ancrage de la légende
    dot.edge('Legend', root_name, style='invis', minlen='1')

    added_edges = set()

    for path_str in paths:
        path = Path(path_str)
        parts = path.parts

        for i, part in enumerate(parts):
            child_node_path = "/".join(parts[:i+1])
            parent_node_path = "/".join(parts[:i]) if i > 0 else root_name

            is_folder = len(Path(part).suffix) == 0

            if is_folder:
                dot.node(
                    child_node_path,
                    label=part + "/",
                    fillcolor=FOLDER_COLOR['fill'],
                    fontcolor=FOLDER_COLOR['font'],
                    color=FOLDER_COLOR['border'],
                    penwidth='1.5',  # Bordure plus épaisse
                    fontname='Arial-Bold',
                    # fontsize hérité de node_attr (16)
                )
            else:
                bg_color, txt_color = get_file_style(part)
                dot.node(
                    child_node_path,
                    label=part,
                    fillcolor=bg_color,
                    fontcolor=txt_color
                )

            edge_key = (parent_node_path, child_node_path)
            if edge_key not in added_edges:
                if parent_node_path == root_name and i == 0:
                    dot.edge(root_name, child_node_path)
                elif i > 0:
                    dot.edge(parent_node_path, child_node_path)
                added_edges.add(edge_key)

    try:
        dot.render(output_filename, view=False, cleanup=True)
        print(f"Graphique format impression généré : {output_filename}.png")
    except Exception as e:
        print(f"Erreur : {e}")


if __name__ == "__main__":
    create_file_tree_graph(file_tree_text)
