---
header-includes:
	\usepackage[czech]{babel}
	\usepackage{a4wide}
---
# Letáková kampaň
## Řešení
### Algoritmus
Úlohu řeším tímto způsobem. Prvně si najdu každou cestu, hledání provádím pomocí alogoritmu hledání do šířky. Na mapě 
města si u každého pole zaznamenám kolik cest přes něj vede. Poté už na každé pole, mimo překážky aplikuji vzorec 
v zadání a vypíšu obrázek. Oblíbenosti směrů u jednotlivých skupin měst řeším takto. Při hledání cesty zařazuji další 
kroky v pořadí oblíbenosti. Například u první skupiny měst začnu krokem nahoru, až pak přidám krok dolů, a tak dále.

### Složitost
Hledání v grafu má složitost `N`, jelikož každé místo navštívím pouze jednou. Pak následuje reverzní hledání cesty se 
složitostí opět `N`. Pokud to shrnu dohromady je celková složitost `2N`^2^. `2N` na jeden graf * `N` cest. Pro vypsání 
obrázku je složitost `O(N`^2^`)` jednou projdu celé město, abych zjistil nejvyšší počet cest na jednom místě, podruhé, 
abych vypsal obrázek.
Po vynecháni zbytečností je složitost.\
`O(N`^2^`)`
