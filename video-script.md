# Videoscript – Project Computer Graphics (OpenGL)

**Vak:** Computer Graphics & Visual Computing  
**Academiejaar:** 2025–2026  
**Denis Topallaj & Maxim Peeters**

---

## Intro

**Denis:**

> We hebben een 3D-omgeving gebouwd in OpenGL met C++ waarbij we een trein over een traject van Bézier-curves laten rijden.

## Feature 1 – Bézier-curves

**Denis:**

> Onze eerste feature zijn de Bézier-curves. Het traject van de trein is opgebouwd uit een aaneenschakeling van kubische Bézier-segmenten die samen een gesloten lus vormen. De scène bevat twee verschillende lussen — één met een grote straal en één met een kleinere maar steilere lus. Om een volledige lus te benaderen wordt er gebruik gemaakt van een waarde die u op het scherm ziet. Aan de hand van deze waarde en een straal, worden er vier kubische segmenten gemaakt zodat het einde van een vorig segment precies samenvalt met het begin van het volgend segment.

_(Toon de witte curvelijnen in de scène.)_

---

## Feature 2 – Animatie met constante snelheid

**Denis:**

> Onze tweede feature is de animatie met constante snelheid via **Arc Length Parametrization**. Omdat de parameter *t* de curve niet lineair doorloopt, bouwen we een **Look-Up Table (LUT)** van 400 samples die de afgelegde afstand koppelt aan *t*. In de render-loop verhogen we een afstand-accumulator met `snelheid * deltaTime`. De functie `getTFromDistance()` gebruikt vervolgens **lineaire interpolatie** in de Look-Up Table om de exacte *t*-waarde te vinden voor de huidige afstand, wat resulteert in een perfect gelijkmatige beweging.

_(Demonstreer de trein die vloeiend en gelijkmatig beweegt langs het traject.)_

---

## Feature 3 – 3D-modellen en texturen

**Denis:**

> Voor de 3D-modellen gebruiken we een box-mesh van 36 vertices. Elke vertex bevat een **positie, normaalvector en UV-coördinaat**. De texturen laden we in via de `stb_image` library.

_(Toon het treinmodel en de textuur van dichtbij.)_

---

## Feature 4 – Visualisatie van het spoor

**Denis:**

> De spoorvisualisatie in de functie `drawRailroad()` werkt door het berekenen van een **lokaal coördinatensysteem** op de curve. Per stap evalueren we de **tangens** (afgeleide) van de curve als de voorwaartse vector. Via kruisproducten met de *up-vector* bepalen we de *right-vector*, waarna we een nieuwe *up-vector* berekenen voor een orthogonaal systeem. Deze drie vectoren vormen de basis van een **4x4 rotatiematrix** die we meegeven aan de shader om elke dwarsligger correct te oriënteren ten opzichte van de baan.

_(Toon het spoor dat vloeiend meebuigt doorheen de scène.)_

---

## Feature 5 – Camera: overzicht & first-person

**Denis:**

> Onze camera ondersteunt drie modi. De berekening van de camera-vectoren gebeurt via Euler-hoeken (Yaw en Pitch), die we omzetten naar een **Front-vector** via goniometrische functies. In de **vrij-beweegbare modus** vangen we muis-offsets op via de `glfw` library. Voor de **First-Person View (FPV)** koppelen we de camerapositie direct aan de positie van de middelste wagon op de Bézier-curve, inclusief een hoogte-offset van 1.0 eenheid boven de baan, anders kom je in de trein terecht.

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
