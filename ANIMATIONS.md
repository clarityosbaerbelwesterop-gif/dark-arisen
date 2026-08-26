# ANIMATIONS.md

## Eiserne Regeln
1. Nur `transform` und `opacity`. Kein `width`, `height`, `top`, `left`, kein animiertes `box-shadow`.
2. Keine Animation länger als 300 ms. Ausnahme: Prestige-Sequenz (1 200 ms) und Phasenwechsel (900 ms).
3. `prefers-reduced-motion: reduce` → alle Bewegungen aus, nur Deckkraft-Übergänge bleiben.
4. Der Hauptzähler im HUD wird **nicht über React gerendert**. Ein `useRef` schreibt direkt in `textContent`. Sonst rendert der Baum 10× pro Sekunde neu und das Gerät wird warm.

## Katalog
| Element | Bewegung | Dauer | Kurve |
|---|---|---|---|
| Geldzähler | zum Zielwert interpolieren | 300 ms | `easeOut` |
| Kaufbutton gedrückt | `scale(0.96)` | 80 ms | `linear` |
| Kauf erfolgreich | `scale(1.05)` → `1` | 180 ms | `easeOut`, Rückstoß |
| Slot füllt sich | Deckkraft 0→1 plus `translateY(4px)→0` | 220 ms | `easeOut` |
| Schwebendes „+X €" | `translateY(0→-40px)`, Deckkraft 1→0 | 700 ms | `easeOut` |
| Fortschrittsbalken | `scaleX` | 200 ms | `linear` |
| Lastleiste Wärme > 85 % | Deckkraft 1↔0,55 im Wechsel | 900 ms | `easeInOut`, endlos |
| Neue Karte freigeschaltet | `translateY(16px)→0` plus kurzer Cyan-Rand | 260 ms | `easeOut` |
| Tab-Wechsel | Kreuzblende der Deckkraft | 150 ms | `linear` |
| Modal | von unten, `translateY(100%)→0` | 240 ms | `easeOut` |

## Phasenwechsel (900 ms)
Bildschirm dunkelt auf 30 % ab, der Phasenname fährt zentriert ein, das Rack wächst um die neuen Slots, dann Rückblende. Einmal Haptik `success`. Das ist der Moment, den Spieler in Bewertungen erwähnen — hier darf es teuer aussehen.

## Prestige (1 200 ms)
Weißer Blitz auf 100 % in 120 ms, alle Zähler zählen sichtbar auf null herunter, Erkenntnis-Zahl fährt aus der Mitte hoch, Rack baut sich in umgekehrter Reihenfolge ab. Danach direkt spielbar, keine Bestätigung mehr.

## Haptik
`light` bei jedem Kauf, `success` bei Modellabschluss und Phasenwechsel, `heavy` nur beim Prestige. Nie bei Tippen in Phase 1 — das wäre Dauervibration.

## Leistungsgrenze
Zielwert 60 fps auf einem iPhone 12. Maximal 12 gleichzeitig laufende Animationen; die schwebenden Zahlen laufen über einen Pool von 8 wiederverwendeten Knoten, nicht über neue Elemente. Wird der Wert gerissen, fliegt zuerst die Pulsanimation der Lastleiste raus, nicht der Zähler.
