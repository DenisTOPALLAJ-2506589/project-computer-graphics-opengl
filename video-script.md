# Videoscript – Project Computer Graphics (OpenGL)

**Vak:** Computer Graphics & Visual Computing  
**Academiejaar:** 2025–2026  
**Denis Topallaj & Maxim Peeters**

---

## Intro

**Denis:**

> Welkom bij onze presentatie over ons OpenGL-project. Voor dit vak hebben we een dynamische 3D-omgeving ontwikkeld in C++. De kern van onze applicatie draait om een trein die over een complex traject van Bézier-curves rijdt. We hebben ons gefocust op zowel technische accuraatheid in de wiskunde achter de curves als op een visueel aantrekkelijke weergave met verschillende camera-standpunten en belichtingseffecten.

## Feature 1 – Bézier-curves

**Denis:**

> Onze eerste belangrijke feature is de implementatie van de Bézier-curves. Het traject dat de trein volgt is niet zomaar een simpele cirkel, maar een aaneenschakeling van meerdere kubische Bézier-segmenten die naadloos op elkaar aansluiten om een gesloten lus te vormen. In de scène zie je twee verschillende trajecten: een wijde lus met lichte hoogteverschillen en een kleinere, meer uitdagende route met steilere hellingen. 

> Om deze lussen wiskundig correct te benaderen, maken we gebruik van een specifieke constante voor cirkelbenadering, gevormd door de verhouding tussen de straal van de cirkel en de lengte van de controlepunten. De formule is te zien op uw scherm. Hiermee berekenen we de controlepunten voor vier segmenten per curve, zodat de overgangen continu zijn en de trein geen schokken vertoont bij het passeren van de knooppunten.

_(Toon de witte curvelijnen in de scène.)_

---

## Feature 2 – Animatie met constante snelheid

**Denis:**

> Een uitdaging bij Bézier-curves is dat de parameter *t* de curve niet met een constante snelheid doorloopt; in scherpere bochten zou de trein normaal gesproken vertragen of versnellen. Om dit op te lossen hebben we **Arc Length Parametrization** toegepast. 

> We bouwen bij het opstarten een **Look-Up Table (LUT)** op met 400 samples, die de cumulatieve booglengte koppelt aan de parameter *t*. Tijdens het renderen houden we de afgelegde afstand bij op basis van de verstreken tijd. Met de functie `getTFromDistance()` zoeken we vervolgens via **lineaire interpolatie** de corresponderende *t*-waarde in onze tabel. Dit zorgt ervoor dat de trein, ongeacht de kromming van de baan, met een perfect constante snelheid van precies 5 eenheden per seconde blijft rijden.

_(Demonstreer de trein die vloeiend en gelijkmatig beweegt langs het traject.)_

---

## Feature 3 – 3D-modellen en texturen

**Denis:**

> Wat betreft de visualisatie maken we gebruik van modulaire 3D-modellen. We hebben een 'Mesh'-klasse geschreven die vertex-data inlaadt, bestaande uit **posities, normalen voor de belichting en UV-coördinaten voor de texturen**. De treinwagon die je hier ziet, is een geëxtrudeerde box-mesh die we hergebruiken voor alle zes de wagons. 

> Voor de textures gebruiken we de `stb_image` bibliotheek om PNG-bestanden in te laden en te binden aan de OpenGL texture units.

_(Toon het treinmodel en de textuur van dichtbij.)_

---

## Feature 4 – Visualisatie van het spoor

**Denis:**

> De spoorvisualisatie in de functie `drawRailroad()` werkt door het berekenen van een **lokaal coördinatensysteem** op de curve. Per stap evalueren we de **tangens** (afgeleide) van de curve als de voorwaartse vector. Via kruisproducten met de *up-vector* bepalen we de *right-vector*, waarna we een nieuwe *up-vector* berekenen voor een orthogonaal systeem. Deze drie vectoren vormen de basis van een **4x4 rotatiematrix** die we meegeven aan de shader om elke dwarsligger correct te oriënteren ten opzichte van de baan.

_(Toon het spoor dat vloeiend meebuigt doorheen de scène.)_

---

## Feature 5 – Camera: overzicht & first-person

**Denis:**

> Ten slotte hebben we een flexibel camerasysteem geïmplementeerd. De gebruiker kan met de toetsen **B** en **F** wisselen tussen verschillende modi. In de **Free-look modus** kun je vrij door de scène vliegen met WASD of ZQSD, waarbij de kijkrichting wordt berekend op basis van muisbewegingen en Euler-hoeken. 

> De meest immersieve stand is echter de **First-Person View**. Hierbij 'locken' we de camera direct op de middelste wagon van de trein. We gebruiken de positie-evaluatie van de Bézier-curve en voegen daar een verticale offset aan toe zodat de camera zich net boven het dak van de wagon bevindt. Omdat de camera de positie en indirect de oriëntatie van de wagon volgt, krijg je als kijker echt het gevoel dat je meerijdt over het golvende spoor.

_(Schakel tussen de camera-modi tijdens de demonstratie.)_

---

## Feature 6 – Belichting

**Maxim:**

> De belichting is gebaseerd op het **Phong-reflectiemodel**, geïmplementeerd in de fragment shader. We gebruiken **vier puntlichten** gedefinieerd in een array van structs. Voor elk licht berekenen we de ambient, diffuse en speculaire componenten op basis van de materiaal-eigenschappen en de `viewPos`. Om realisme toe te voegen passen we **kwadratische attenuatie** toe, waardoor het licht vloeiend wegvalt naarmate de afstand tot de bron groter wordt.

_(Toon de lichtbronnen naast het spoor en de belichting op de trein.)_poor en de belichting op de trein.)_

---

## Feature 7 – Convolutie-shader

**Maxim:**

> Als zevende feature hebben we een **post-processing convolutieshader** geïmplementeerd. Dit werkt via een **tweefasen-aanpak**. In de eerste fase renderen we de scène naar een **Framebuffer Object (FBO)**. In de tweede fase tekenen we een **screen-aligned quad**, wat een rechthoek van twee driehoeken in Normalized Device Coordinates (NDC) is, dus van -1 tot +1. De fragment shader sampelt de kleurbuffer van de FBO met de `texture()` functie en past een **3×3 convolutiekernel** toe. Onze **verscherpingskernel**, met een centrumgewicht van 7, versterkt contrasten tussen naburige pixels voor een scherper eindresultaat.

_(Toon het resultaat met de verscherpingsfilter actief.)_

---

## Feature 10 – Interactie via Picking

**Maxim:**

> Tot slot de interactie via **picking**. Om de 3D-positie van de oranje knop te koppelen aan een muisklik, gebruiken we `glm::project()` zodat we de world-space coördinaten kunnen transformeren naar **schermcoördinaten**. Daarbij inverteren we de Y-as omdat OpenGL zijn oorsprong linksonder plaatst, terwijl Window-coördinaten linksboven beginnen. Als de afstand tussen de cursor en de geprojecteerde positie kleiner is dan 25 pixels, wisselen we van traject en resetten we de `bezierDistance`, zodat de trein netjes aan het begin van de nieuwe lus start. Dit kan gebeuren aan de hand van de muisklik op het oranje object of door op de C-toets te drukken.

_(Klik op de oranje knop en druk op C om te tonen hoe de trein van traject wisselt.)_

---

## Afsluiting

**Maxim:**

> We hebben het volgende niet gedaan, namelijk het Post-processing met een Bloom/Neon of Halo effect en Chroma-keying. Dit kwam omdat we niet genoeg tijd hadden om dit te implementeren. Bedankt voor uw aandacht.

---

_Einde van het videoscript._
