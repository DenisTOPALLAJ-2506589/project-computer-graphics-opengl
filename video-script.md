# Videoscript – Project Computer Graphics (OpenGL)

**Vak:** Computer Graphics & Visual Computing  
**Academiejaar:** 2025–2026  
**Denis Topallaj & Maxim Peeters**

---

## Intro

**Denis:**
> We hebben een 3D-omgeving gebouwd in OpenGL met C++ waarbij een trein over een traject van Bézier-curves rijdt.

---

## Feature 1 – Bézier-curves

**Denis:**
> We starten met de curves. Het traject van de trein is opgebouwd uit een aaneenschakeling van kubische Bézier-segmenten die samen een gesloten lus vormen. De scène bevat twee verschillende lussen — één met een grote straal en één met een kleinere maar steilere lus. De controlepunten worden procedureel gegenereerd met de bekende benadering `k = 0.552 * r`, zodat vier kubische segmenten samen een volledige cirkel benaderen. Je ziet het traject hier als een witte lijn, opgebouwd uit 400 samplepunten.

*(Toon de witte curvelijnen in de scène.)*

---

## Feature 2 – Animatie met constante snelheid

**Denis:**
> Als we de trein gewoon met een constante verhoging van parameter *t* over de curve laten rijden, dan merken we dat de snelheid niet constant is — het object versnelt en vertraagt afhankelijk van de vorm van de curve. Om dit op te lossen hebben we per curve een **look-up table** opgebouwd die de booglengte koppelt aan de curvewaarde *t*. Elke frame hogen we een afstandsaccumulator op met `deltaTime * 5` eenheden per seconde. Via lineaire interpolatie in die tabel halen we het correcte *t*-waarde op, zodat de trein altijd met een constante snelheid rijdt.

*(Demonstreer de trein die vloeiend en gelijkmatig beweegt langs het traject.)*

---

## Feature 3 – 3D-modellen en texturen

**Denis:**
> De trein zelf is een rechthoekig boxmodel opgebouwd uit 36 vertices met posities, normaalvectoren en UV-coördinaten. Een PNG-textuur van een treinwagon wordt ingeladen via de `stb_image` library en op het materiaal toegepast in de fragment shader. Hetzelfde boxmodel wordt hergebruikt voor de dwarsliggers van het spoor en voor de oranje wisselknop in de scène.

*(Toon het treinmodel en de textuur van dichtbij.)*

---

## Feature 4 – Visualisatie van het spoor

**Denis:**
> Eén van de mooiste onderdelen is het **treinspoor dat meebuigt met de Bézier-curve**. De functie `drawRailroad()` plaatst om de 0,6 booglengte-eenheden een afgeplatte box als dwarsligger. Voor elke positie berekenen we het lokale Frenet-frame — tangent, up en right — en bouwen we hieruit een rotatiematrix. Die matrix oriënteert de dwarsligger zodat hij altijd loodrecht op de rijrichting staat. Het resultaat is een spoor dat automatisch meebuigt doorheen elke bocht en stijging.

*(Toon het spoor dat vloeiend meebuigt doorheen de scène.)*

---

## Feature 5 – Camera: overzicht & first-person

**Denis:**
> We hebben drie camera-modi geïmplementeerd. In de **standaard modus** beweeg je vrij door de scène met WASD en kijk je rond door de rechtermuisknop ingedrukt te houden. Door op **B** te drukken schakel je de vrije-vliegmodus in met muisbesturing. Door op **F** te drukken activeer je de **first-person camera**, die vergrendeld is aan de middelste wagon. Je ziet het traject dan vanuit het perspectief van de rijdende trein, wat een heel andere beleving geeft.

*(Schakel tussen de camera-modi tijdens de demonstratie.)*

---

## Feature 6 – Belichting

**Maxim:**
> Nu neem ik het over voor de belichting. Er zijn **vier punt-lichtbronnen** symmetrisch rond het spoor geplaatst op posities (+/-8.5, 1, 0) en (0, 1, +/-8.5). De belichting is volledig geïmplementeerd als **per-pixel Phong shading** in de fragment shader — zo wordt voor elke pixel het ambient-, diffuse- en speculaire licht berekend. Elke lichtbron heeft ook **kwadratische attenuatie**, zodat de intensiteit realistisch afneemt met de afstand. De kleine kubusvormige lichtbronnen zijn zichtbaar als witte blokjes in de scène.

*(Toon de lichtbronnen naast het spoor en de belichting op de trein.)*

---

## Feature 7 – Convolutie-shader

**Maxim:**
> Vervolgens hebben we een **post-processing pipeline** opgebouwd. De scène wordt eerst volledig gerenderd naar een **Framebuffer Object (FBO)**. De kleurenbuffer van die FBO gebruiken we daarna als textuur op een screen-aligned quad. Op die quad passen we de `post_sharpen.frag` shader toe, die een **3x3 convolutiekernel** toepast over elk pixel. De gehanteerde kernel is een **verscherpingskernel**, met een centrale gewichtsfactor van 7 en negatieve buurbijdragen. Dit verbetert de helderheid van randen en details in het volledige gerenderde beeld.

*(Toon het resultaat met de verscherpingsfilter actief.)*

---

## Feature 10 – Interactie via Picking

**Maxim:**
> Als laatste interactiemogelijkheid demonstreer ik de **wisselspoor-interactie**. Je kunt van traject wisselen op twee manieren: door op de **C-toets** te drukken, of door met de muis op de **oranje wisselknop** in de scène te klikken. Die knop staat opsteld in de 3D-wereld op positie (0, 3, 0). Bij een muisklik projecteren we die positie naar schermcoördinaten via `glm::project()` en vergelijken we de pixelafstand met de muiscursor — als die kleiner is dan 25 pixels, wisselt de trein van lus. De afstandsaccumulator wordt hierbij gereset naar 0.

*(Klik op de oranje knop en druk op C om te tonen hoe de trein van traject wisselt.)*

---

## Afsluiting

**Maxim:**
> Dat was een overzicht van alle geïmplementeerde features van ons project: Bézier-curves met constante-snelheidsanimatie, een getextureerd treinmodel, een visueel spoor dat meebuigt met de curve, meerdere camera-modi, per-pixel belichting, een post-processing convolutie-shader en interactieve trajectwisseling.

---

*Einde van het videoscript.*
