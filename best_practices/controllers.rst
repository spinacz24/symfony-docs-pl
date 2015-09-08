Kontrolery
==========

Symfony działa zgodnie z filozofią *"chude kontrolery i grube modele"*. Oznacza to,
że kontrolery powinny posiadać tylko chudą warstwę *kodu sklejającego*
potrzebnego do koordynacji różnych części aplikacji.

Jako zasadę należy przyjąć regułę "5-10-20", według której kontrolery powinny
definiować co najwyżej 5 zmiennych, zawierać nie więcej niz 10 akcji i obejmować
co najwyźej 20 linii kodu w każdej akcji. Nie należy tego traktować dosłownie,
ale przestrzeganie tej zasady pomoże, gdy kontroler trzeba refaktoryzować
i przekazać do usługi.

.. tip::

    Wykonuj kontroler jako rozszerzenie bazowej klasy FrameworkBundle i używaj
    adnotacji do konfigurowania trasowania, buforowania i zabezpieczeń, gdy tylko
    jest to możliwe.

Sprzężenie kontrolerów z podstawym frameworkiem pozwala na lewarowanie wszystkich
jego możliwości i zwiększenie produktywności.

Ponieważ kontrolery powinny być chude i zawierać nie więcej niż kilka linii
*kodu sklejajacego*, to spędzanie wielu godzin nad próbowaniem stworzenia
kontrolerów, niezależnych od frameworka, nie przynosi korzyści w dłuższym okresie.
Taki nakład pracy będzie *zmarnowany*.

Ponadto, uzywajac adnostacji dla trasowania, buforowania i zabezpieczeń, upraszcza
się konfiguracje. Nie trzeba przegladać dziesiątek plików utworzonych w różnych
formatach (YAML, XML, PHP): cała konfiguracja jest w jednym miejscu i wykorzystuje
jeden format.

Ogólnie rzecz biorąc, oznacza to, że powinno się oddzielać logikę biznesową od
frameworka, równocześnie łącząc agresywnie swoje kontrolery i trasowanie z frameworkiem,
w celu ich najlepszego wykorzystania.

Konfiguracja trasowania
-----------------------

W celu załadowania w kontrolerach definicji trasowania w postaci adnotacji, trzeba
dodać nastęþująca konfigurację w głównym pliku konfiguracyjnym:

.. code-block:: yaml

    # app/config/routing.yml
    app:
        resource: "@AppBundle/Controller/"
        type:     annotation

Konfiguracja ta spowoduje załadowanie adnotacji z każdego kontrolera zapisanego
w katalogu ``src/AppBundle/Controller/`` oraz z jego podkatalogów.
Jeśli aplikacja definiuje wiele kontrolerów, to jest jak najbardziej w porządku
organizowanie ich w podkatalogi:

.. code-block:: text

    <your-project>/
    ├─ ...
    └─ src/
       └─ AppBundle/
          ├─ ...
          └─ Controller/
             ├─ DefaultController.php
             ├─ ...
             ├─ Api/
             │  ├─ ...
             │  └─ ...
             └─ Backend/
                ├─ ...
                └─ ...

Konfiguracja szablonu
---------------------

.. tip::

    Nie uzywaj adnotacji ``@Template()`` do konfigurowania szablonu stosowanego
    w kontrolerze.

Adnotacja ``@Template`` jest pozyteczna, ale równocześnie angażuje trochę magii.
Sądzimy, że przydatność tej adnotacji nie równoważy problemów związanych z tą magią,
więc odradzamy wykorzystywanię adnotacji ``@Template``.

W wiekszości przypadków, ``@Template`` jest stosowany bez jakichkolwiek parametrów,
co sprawia, że trudniej jest się zorientować, który szablon jest renderowany.
Adnotacja ta zaciemnia też (szczególnie osobom początkującym) zasade, że kontroler
zawsze powinien zwracać obiekt Response (chyba, że wykorzystuje sie warstwę widoku).  

Jak wygląda kontroler
---------------------

Biorąc to wszystko po uwagę, przedstawmy przykładowy kod kontrolera dla strony
początkowej aplikacji:

.. code-block:: php

    namespace AppBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;

    class DefaultController extends Controller
    {
        /**
         * @Route("/", name="homepage")
         */
        public function indexAction()
        {
            $posts = $this->getDoctrine()
                ->getRepository('AppBundle:Post')
                ->findLatest();

            return $this->render('default/index.html.twig', array(
                'posts' => $posts
            ));
        }
    }

.. _best-practices-paramconverter:

Używanie ParamConverter
-----------------------

Jeśli używasz Doctrine, to możesz *ewentualnie* wykorzystać `ParamConverter`_
do automatycznego wykonywania zapytania dla encji i przekazywania go jako
argument do kontrolera.

.. tip::

    Używaj triku ParamConverter do automatycznego wykonywania zapytania dla encji
    Doctrine, kiedy jest to proste i wygodne.

Na przykład:

.. code-block:: php

    use AppBundle\Entity\Post;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;

    /**
     * @Route("/{id}", name="admin_post_show")
     */
    public function showAction(Post $post)
    {
        $deleteForm = $this->createDeleteForm($post);

        return $this->render('admin/post/show.html.twig', array(
            'post'        => $post,
            'delete_form' => $deleteForm->createView(),
        ));
    }

Zwykle w ``showAction`` oczekuje się argumentu ``$id``. Zamiast tego, tworząc
nowy argument (``$post``) o typie klasa ``Post``(która jest encją Doctrine),
ParamConverter automatycznie odpytuje bazę danych o obiekt, którego właściwość
``$id`` pasuje do wartosci ``{id}``. Kod ten również pokazuje stronę 404, jeśli
żaden obiekt ``Post`` nie może być znaleziony.

Co kiedy rzecz się robi bardziej zaawansowana?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Powyżej omówiony kod działa bez jakiejkolwiek konfiguracji, ponieważ wieloznacznik
``{id}``dopasowuje nazwy właściwości w encji. Jeśli tak nie jest lub jeśli ma się
jeszcze bardziej złożoną logikę, najprościej jest wypytać encję ręcznie.
W naszej aplikacji mamy taka sytuacje w ``CommentController``:

.. code-block:: php

    /**
     * @Route("/comment/{postSlug}/new", name = "comment_new")
     */
    public function newAction(Request $request, $postSlug)
    {
        $post = $this->getDoctrine()
            ->getRepository('AppBundle:Post')
            ->findOneBy(array('slug' => $postSlug));

        if (!$post) {
            throw $this->createNotFoundException();
        }

        // ...
    }

Można również użyć konfiguracji ``@ParamConverter``, która jest bardzo elastyczna:

.. code-block:: php

    use AppBundle\Entity\Post;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\ParamConverter;
    use Symfony\Component\HttpFoundation\Request;

    /**
     * @Route("/comment/{postSlug}/new", name = "comment_new")
     * @ParamConverter("post", options={"mapping": {"postSlug": "slug"}})
     */
    public function newAction(Request $request, Post $post)
    {
        // ...
    }

Wnosek: skrót ParamConverter jest świetny dla prostych sytuacji. Trzeba jednak
pamiętać, że bezpośrednie wypytywanie encji jest też bardzo łatwe.

Haki wczesne i późne
--------------------

Jeśli zachodzi potrzeba wykonywania jakiegośc kodu przed lub po wykonaniu kontrolerów,
można użyć komponentu EventDispatcher do
:doc:`ustawienia wczesnych lub późnych filtrów </cookbook/event_dispatcher/before_after_filters>`.

.. _`ParamConverter`: https://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/annotations/converters.html
