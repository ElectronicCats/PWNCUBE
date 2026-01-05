
# Plantilla para proyectos de ingeniería

Esta plantilla es la base para cualquier proyecto desarrollado en Electronic Cats.

Automáticamente, se generarán todos los archivos necesarios para la compra de material, fabricación y ensamble.

Así como documentos complementarios para un proyecto completo.

  

> Este README.md puede ser utilizado como plantilla para documentación, de esta manera se puede incluir generalidades, recomendaciones y todo lo necesario para entender el proyecto.

  

## ¿Cómo utilizar esta plantilla?

Para comenzar un nuevo proyecto, presiona el botón de "Use this template".

  

### KiCad

Para esta plantilla, el hardware debe de ser diseñado y/o desarrollado en KiCad 9.

Al término del diseño del proyecto, KiCad deberá de generar los siguientes archivos:

  

- nombre_del_proyecto.kicad_pro

- nombre_de_la_pcb.kicad_pcb

- nombre_del_esquematico.kicad_sch

  

Además de archivos temporales, los cuales Git ignora al momento de cualquier push.

[Archivos ignorados](.gitignore)

  

Estos archivos deberán ser guardados dentro de la carpeta de [hardware](hardware/).

  

### Configuración de automatización

Los workflows de GitHub Actions detectan automáticamente los archivos de KiCAD en el directorio `hardware/`. No es necesario modificar ningún archivo de configuración.

Los workflows buscarán automáticamente:
- El primer archivo `.kicad_pcb` encontrado en `hardware/`
- El archivo `.kicad_sch` que coincida con el nombre del PCB
- Si no hay coincidencia exacta, usará el primer `.kicad_sch` disponible

**Nota**: Asegúrate de que los archivos de KiCAD estén guardados en la carpeta `hardware/` con sus extensiones correctas (`.kicad_pro`, `.kicad_pcb`, `.kicad_sch`).

  

### Activar/desactivar DRC y ERC

Las opciones de DRC y ERC están siempre activas predeterminadamente. Para desactivarlas, se deberá modificar el archivo [electroniccats_sch.kibot.yaml](hardware/electroniccats_sch.kibot.yaml) cambiando las siguientes líneas:

```yaml

run_erc: false  # Cambiar a false para desactivar ERC

run_drc: false  # Cambiar a false para desactivar DRC

```

**Workflows disponibles:**
- **DRC y ERC**: Se ejecutan automáticamente en cada `push` y `pull_request` para validar el diseño
- **Archivos de fabricación**: Se generan automáticamente cuando se crea un `release` publicado

## Creación de Release

Al terminar el proyecto y su revisión, se publicará el primer Release.

Para crear un nuevo Release, presiona el botón de "Create a new release".

Una vez creado el Release, podrás ver la creación de los archivos en la sección de Actions.

Al terminar, los archivos serán generados en el mismo release.

## Elementos para mejorar tu `Readme.md`

Los archivos `Readme.md` se crean con el propósito de hacer visualmente agradable un repositorio para los usuarios que visiten nuestro proyecto, puedes utilizar algunos de los siguientes elementos.

### Código
Quoting code, como lo dice su nombre, se utiliza para añadir código y que se separe del texto plano. Debes de agregar el codigo dentro de ` ``` ```` ` , ademas si agregas el nombre del lenguaje inmediatamente despues de las primeras ` ``` ` las funciones se pondran de color diferenciandolas del resto del codigo. Aquí algunos ejemplos. Puedes encontrar los lenguajes aquí:
https://github.com/github/linguist/blob/master/vendor/README.md
```sh
192.168.0.1
cd Downloads
```
  ```diff
- text in red
+ text in green
! text in orange
# text in gray
@@ text in purple (and bold)@@
```

### Hipervínculos

Usa hipervínculos o links para redirigir a los usuarios a páginas donde puedan conocer más acerca de algún tema o concepto en concreto, hay dos formas de hacer esto:

- [Agregando el link en seguida](https://github.com/ElectronicCats/Template-Project-KiCAD-CI) - Con esta forma deberas de seguir el siguiente formato `[Texto](www.url.com)`. El texto que se mostrará en la página principal del Readme será el que se encuentra dentro de los corchetes y el link de la página deberá de ir de manera inmediata a los corchetes dentro de paréntesis.

- [Agregando el link como referencia] - Al igual que otro tipo de formatos de referencia en este agregas el texto que se mostrará en la página principal del repositorio dentro de corchetes `[Texto]` y al final de tu archivo (de preferencia) agregas la referencia de la siguiente manera: `[Texto]:<www.url.com>`, este no se mostrara en el archivo por lo que es una buena forma de mantener un formato y un orden.  

### Tablas

Las tablas que todos conocemos con filas y columnas. El formato para estas tablas se basa en el uso del símbolo `|`, entonces debes de encerrar las palabras como esto: `|Columna1|` (sin la posibilidad de dos | seguidos), para agregar más columnas basta con dar un espacio y repetir el formato, sin embargo, para añadir filas debes de hacer un salto de línea y repetir el formato de columnas, dejándonos una tabla como la siguiente:

|Columna1|Columna2|
|-|-|
|Fila 1 Columna 1|Fila 2 Columna 2|

Nota: Si agregas en la segunda fila guiones (-) harás que la primera fila se convierta en el encabezado de la tabla.

### Imágenes
Puedes añadir imágenes siempre y cuando estas estén en Internet, si quieres agregar una nueva imagen tambien la puedes arrastrar y soltar en el cuadro de texto (en caso de que edites tu `Readme.md`) directo desde GitHub, esto hará que se guarde tu imagen en una carpeta oculta dentro de tu repositorio.
El formato para agregar imagenes es: `![](www.urlimagen.com)`
Si requieres que al hacer click en tu imagen se redirija a otra pagina usa el siguiente formado `[![](www.urldeimagen.com)](https://www.urlaredirigir.com)`
Es importante agregar `https://` , si no te enviara a una página de GitHub que probablemente no exista.

[![](https://electroniccats.com/wp-content/uploads/2018/01/fav.png)](https://www.electroniccats.com)

### Referencias
Es posible que en la wiki hayas visto numeritos como este --->[^1], pero que significan?

No son más que referencias que puedes hacer para hacer saltos de información e ir directo a la referencia haciendo click en el pequeño número.
[^1]: Soy la referencia :))))

### Emojis :trollface: :shipit:
Solo escribe el código del emoji así: `:EMOJICODE:`.
Aqui la lista de los [EMOJICODEs](https://github.com/ikatyang/emoji-cheat-sheet/blob/master/README.md#github-custom-emoji)

### Listas con checkbox
Usa este formato:
```
- [x] GFM task list 1
- [x] GFM task list 2
- [ ] GFM task list 3
    - [ ] GFM task list 3-1
    - [ ] GFM task list 3-2
    - [ ] GFM task list 3-3
- [ ] GFM task list 4
    - [ ] GFM task list 4-1
    - [ ] GFM task list 4-2
  ```
  Y tendras algo asi: 
- [x] GFM task list 1
- [x] GFM task list 2
- [ ] GFM task list 3
    - [ ] GFM task list 3-1
    - [ ] GFM task list 3-2
    - [ ] GFM task list 3-3
- [ ] GFM task list 4
    - [ ] GFM task list 4-1
    - [ ] GFM task list 4-2

### Otros Elementos

![](https://img.shields.io/github/stars/ElectronicCats/Template-Project-KiCAD-CI?style=for-the-badge)
![](https://img.shields.io/github/forks/ElectronicCats/Template-Project-KiCAD-CI?color=green&style=for-the-badge)

Este tipo de indicadores nos pueden ayudar a identificar diferente información relacionada al proyecto, solo los debes de agregar como una imagen y en el URL  pegar el link correspondiente. 
Los ejemplos de arriba fueron generados con la pagina: Shields.io , solo debes de asegurarte que son para GitHub y que tienen el formato `MarkDown`

- Badges,
En caso de que necesites algun referente a alguna empresa o plataforma puedes usar esta pagina: https://dev.to/envoy_/150-badges-for-github-pnk

[![](https://img.shields.io/badge/LinkedIn-0077B5?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/company/electroniccats/?originalSubdomain=mx)

- Estadísticas
Utilizando el projecto de este usuario puedes agregar estadísticas del proyecto como estas:

![This repository Stats](https://github-readme-stats.vercel.app/api/pin?username=ElectronicCats&repo=Template-Project-KiCAD-CI&title_color=fff&icon_color=f9f9f9&text_color=9f9f9f&bg_color=151515)

https://github.com/anuraghazra/github-readme-stats

**NOTA**:Son pocas las tarjetas que puedes utilizar con el proyecto de este usuario para repositorios ya que son más dirigidos a perfiles de GitHub.
 
## Maintainer

<a
href="https://github.com/sponsors/ElectronicCats">

<img  src="https://electroniccats.com/wp-content/uploads/2020/07/Badge_GHS.png"  height="104" />

</a>

Electronic Cats invests time and resources providing this open source design, please support Electronic Cats and open-source hardware by purchasing products from Electronic Cats!

[Agregando el link como referencia]: <https://github.com/ElectronicCats/Template-Project-KiCAD-CI>
