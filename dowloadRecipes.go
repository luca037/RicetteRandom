package main

import (
	"encoding/xml"
	"log"
	"os"
	"strings"

	gzut "github.com/luca037/RicetteRandom/gialloZafferanoUtils"
	"golang.org/x/net/html"
)

// Indirizzo url da cui partono le richieste (prima pagina ricetta). (DA SETTARE)
const kURL = "https://www.giallozafferano.it/ricette-cat/Bevande/"

// Totale pagine della ricetta dell'url (numero in fondo alla prima pagina).
// Fissa la dimensione di alcuni channels, non indica il numero di pagine
// da cui vengono estratte le ricette. (DA SETTARE)
const kTOT_PAGES = 6

// Numero di ricette per pagina (solitamente è 15). Fissa la dimensione di
// alcuni channels.
const kRECIPES_PER_PAGE = 15

// Path cartella in cui salvare le ricette estratte. (DA SETTARE)
const kOUT_PATH_DIR = "ricette/bevande/"

// Path file contenente i path delle cartelle contenenti le ricette.
const kRECIPES_DIRS_PATHS = "recipesDirsPaths.txt"

func main() {
	// salvo i link relativi alle pagine (pagine e ricette) in due channels
	pagesUrls := make(chan string, kTOT_PAGES)                         // url pag contenenti gli url delle ricette
	pagesUrls <- kURL                                                  // url di partenza
	pageHeads := make(chan *html.Node, len(pagesUrls))                 // head html pag contenenti url delle ricette
	recipesUrls := make(chan string, kRECIPES_PER_PAGE*len(pagesUrls)) // url delle ricette
	recipesHeads := make(chan *html.Node, len(recipesUrls))            // head html ricette
	recipesToSave := make(chan gzut.Recipe, len(recipesUrls))          // Recipe da serializzare in xml

	go gzut.MkeRequest(pagesUrls, pageHeads)
	go gzut.ParseRecipesPage(pageHeads, recipesUrls, pagesUrls)
	go gzut.MkeRequest(recipesUrls, recipesHeads)
	go gzut.ParseRecipe(recipesHeads, recipesToSave)

	// salvo dove si trova la cartella contenente i file xml (serve al programma in C++)
	file, err := os.OpenFile(kRECIPES_DIRS_PATHS, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()
	file.Write([]byte(os.Getenv("PWD") + "/" + kOUT_PATH_DIR + "\n")) // abs path

	// serializzazione ricette in file formato xml
	if err := os.MkdirAll(kOUT_PATH_DIR, os.ModePerm); err != nil {
		log.Fatal(err)
	}
	for r := range recipesToSave {
		file, err := os.Create(kOUT_PATH_DIR + rmSomePunctuation(r.Name) + ".xml")
		if err != nil {
			log.Fatal(err)
		}
		defer file.Close()
		encoder := xml.NewEncoder(file)
		encoder.Indent("", "    ")
		if err := encoder.Encode(r); err != nil {
			log.Fatal(err)
		}
	}
}

// Ritorna una stringa in cui non sono presenti i segni di punteggiatura specificati.
func rmSomePunctuation(in string) string {
	var out strings.Builder
	for _, ch := range in {
		if ch == '"' || ch == ':' || ch == ',' || ch == '.' { // TODO: Passare la punteggiatura tramite argomento
			continue
		} else if ch == ' ' {
			out.WriteRune('_')
			continue
		}
		out.WriteRune(ch)
	}
	return out.String()
}
