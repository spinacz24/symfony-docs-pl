.. highlight:: php
   :linenothreshold: 2

.. index::
   single: formularze

Formularze
==========

Radzenie sobie z formularzami HTML jest jednym z najczęstszych (i trudnych) zadań,
z jakimi musi się uporać twórca aplikacji internetowych. Symfony integruje z sobą
komponent *Form*, który znacznie ułatwia pracę z formularzami. W tej części podręcznika
zbudujemy od podstaw skomplikowany formularz, ucząc się stopniowo najważniejszych
funkcjonalności biblioteki formularzy.

.. note::

   `Komponent Form`_ jest niezależną biblioteką, która może zostać użyta
   poza projektami Symfony. Więcej informacji znajdziesz w dokumentacji
   :doc:`komponentu Form </components/form/introduction>` dostępnej też na Github.

.. index::
   single: formularze; utworzenie prostego formularza

Utworzenie prostego formularza
------------------------------

Załóżmy, że budujemy aplikację prostej listy zadań do zrobienia, która będzie
wykorzystywana do wyświetlania "zadań". Ponieważ nasi użytkownicy będą mieli
potrzebę edytowania i tworzenia zadań, to musimy zbudować formularz. Zacznijmy
od utworzenia ogólnej klasy ``Task``, która będzie reprezentować
i przechowywać dane dla pojedynczego zadania::

       // src/AppBundle/Entity/Task.php
    namespace AppBundle\Entity;

    class Task
    {
        protected $task;
        protected $dueDate;

        public function getTask()
        {
            return $this->task;
        }

        public function setTask($task)
        {
            $this->task = $task;
        }

        public function getDueDate()
        {
            return $this->dueDate;
        }

        public function setDueDate(\DateTime $dueDate = null)
        {
            $this->dueDate = $dueDate;
        }
    }
    
Klasa ta, jak dotychczas, nie ma nic wspólnego z Symfony lub jakąkolwiek biblioteką.
Jest zwykła klasa PHP, która bezpośrednio rozwiązuje problem wewnątrz naszej
aplikacji (czyli potrzebę reprezentowania zadania na użytek aplikacji). Pod koniec tego
rozdziału będziemy w stanie zgłaszać dane do obiektu ``Task`` (poprzez formularz HTML),
weryfikować swoje dane i utrwalać je w bazie danych.

.. index::
   single: formularze; tworzenie formularza w kontrolerze

Budowanie formularza
~~~~~~~~~~~~~~~~~~~~

Teraz po utworzeniu klasy ``Task``, następnym krokiem jest utworzenie i zrenderowanie
rzeczywistego formularza HTML . W Symfony odbywa się to przez zbudowanie obiektu
formularza i zrenderowania formularza w szablonie. Na razie wszystko może być zrobione
wewnątrz akcji::

    // src/AppBundle/Controller/DefaultController.php
    namespace AppBundle\Controller;

    use AppBundle\Entity\Task;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\Form\Extension\Core\Type\TextType;
    use Symfony\Component\Form\Extension\Core\Type\DateType;
    use Symfony\Component\Form\Extension\Core\Type\SubmitType;

    class DefaultController extends Controller
    {
        public function newAction(Request $request)
        {
            // tworzymy zadanie i nadajemy mu jakieś dane testowe dla tego przykladu
            $task = new Task();
            $task->setTask('Write a blog post');
            $task->setDueDate(new \DateTime('tomorrow'));

            $form = $this->createFormBuilder($task)
                ->add('task', TextType::class)
                ->add('dueDate', DateType::class)
                ->add('save', SubmitType::class, array('label' => 'Create Task'))
                ->getForm();

            return $this->render('default/new.html.twig', array(
                'form' => $form->createView(),
            ));
        }
    }
    
.. tip::

   Przykład ten pokazuje, jak zbudować formularz bezpośrednio w akcji. Później,
   w rozdziale ":ref:`book-form-creating-form-classes`" dowiemy się, jak zbudować
   formularz w samodzielnej klasie, co jet zalecane, jako że formularz staje się
   wówczas możliwy do wielokrotnego wykorzystania.

Tworzenie formularza wymaga stosunkowo mało kodu, ponieważ obiekty formularza
Symfony są budowane w "konstruktorze formularzy". Celem konstruktora formularzy
jest umożliwienie prostego tworzenia "recept" na formularz i używanie ich do
rzeczywistego budowania formularzy.

W tym przykładzie dodaliśmy do formularza dwa pola, ``task`` i ``dueDate``,
odnoszące się do właściwości ``task`` i ``dueDate`` klasy ``Task``.
Musi się także przypisać każdy "typ" (np. ``TextType`` i ``DateType``),
reprezentowany prez jego w pełni kwalifikowana nazwę klasy. Między innymi, określa
to, czy dla każdego pola są renderowane znaczniki HTML formularza.

.. versionadded:: 2.8
    Do oznaczenia typu formularza w PHP 5.5+ lub
    w ``Symfony\Component\Form\Extension\Core\Type\TextType`, musi się używać
    w pełni kwalifikowanej nazwy klasy, takiej jak ``TextType::class``.
    Przed Symfony 2.8, mozna było użyć alias dla kazdego typu, taki jak ``text``
    lub ``date``. Stara skladnia aliasu bedzie nadal działać do Symfony 3.0.
    Wiecej informacji znajduje sie w pliku `2.8 UPGRADE Log`_.

Na koniec dodamy przycisk zgłaszjący dla przesyłania formularza na serwer.

.. versionadded:: 2.3
    W Symfony 2.3 dodano obsługę przycisków zgłaszających (*submit*). Wcześniej
    trzeba było dodawać przyciski do formularza HTML ręcznie (jak w przykładzie
    w następnym rozdziale).

Symfony posiada wiele wbudowanych typów pól, które zostaną krótko omówione w
rozdziale :ref:`book-forms-type-reference`).

.. index::

   single: formularze; renderowanie

Renderowanie formularzy
~~~~~~~~~~~~~~~~~~~~~~~

Teraz, gdy formularz został stworzony, następnym krokiem jest jego wyrenderowanie.
Realizuje się to przekazując specjalny obiekt "widoku" formularza  do szablonu
(``$form->createView()`` w powyższej akcji) i używając pomocniczych funkcji
formularza:

.. configuration-block::

    .. code-block:: html+twig
       :linenos:

        {# app/Resources/views/default/new.html.twig #}
        {{ form_start(form) }}
        {{ form_widget(form) }}
        {{ form_end(form) }}

    .. code-block:: html+php
       :linenos:

        <!-- app/Resources/views/default/new.html.php -->
        <?php echo $view['form']->start($form) ?>
        <?php echo $view['form']->widget($form) ?>
        <?php echo $view['form']->end($form) ?>

.. image:: /images/book/form-simple.png
   :align: center

.. note::
   W przykładzie tym założono, że wysyła się formularz w żadaniu "POST" i to na
   ten sam adres URL, który został wyświeylony w formularzu. Później dowiesz się,
   jak zmienić metodę ``request`` i docelowy adres URL formularza.

To jest to! potrzeba tylko trzech linii, aby wyrenderować kompletny formularz:

``form_start(form)``
    Renderuje początkowy znacznik formularza, łącznie z prawidłowym atrybutem
    ``enctype``, gdy stosuje się przesyłanie pliku.

``form_widget(form)``
    Renderuje wszystkie pola, w tym sam element pola, etykietę i komunikat błędu
    dla pola.

``form_end(form)``
    Renderuje znacznik końcowy formularza i wszystkie pola, które nie zostały
    jeszcze zrenderowane, w przypadku gdy renderuje się każde pole samodzielnie.
    Jest to przydatne do renderowania ukrytych pól i wykorzystania automatycznego
    :ref:`zabezpieczenia przed CSRF <forms-csrf>`.

.. seealso::

    Jest to bardzo proste, ale nieelastyczne (jeszcze). Zwykle, chce się
    renderować każde pole indywidualnie, aby móc kontrolować wygląd formularza.
    Dowiesz się jak to zrobić w rozdziale ":ref:`form-rendering-template`".

Zanim przejdziemy dalej, zwróć uwagę na to, jak zostało zrenderowane pole wejściowe
``task``, mające wartość właściwości ``task`` obiektu ``$task`` (czyli "Write a blog
post"). Jest to pierwsza czynność formularza – pobranie danych z obiektu
i przetłumaczenie ich na format, który jest odpowiedni do odtworzenia w formularzu HTML.

.. tip::

   System formularza jest wystarczająco inteligentny aby uzyskać dostęp do wartości
   chronionej właściwości ``task``, poprzez metody ``getTask()`` i ``setTask()``
   w klasie ``Task``. Gdy właściwość nie jest publiczna, to musi się użyć metod
   akcesorów, tak aby komponent Form mógł pobierać i zapisywać dane we
   właściwościach. Dla właściwości logicznych można użyć metody "isser" lub "hasser"
   (np. ``isPublished()`` lub ``hasReminder()``) wewnątrz akcesora pobierającego
   (np. ``getPublished()`` lub ``getReminder()``).

.. index::
   single: formularze; obsługa zgłoszeń formularza
  
.. _book-form-handling-form-submissions:

Obsługa zgłoszeń formularza
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Drugą czynnością formularza jest przełożenie przesłanych z powrotem danych
na właściwości obiektu. Aby tak się stało, trzeba powiązać dane przesłane przez
użytkownika z formularzem. W naszej akcji dodamy następujacą rzecz::

    // ...
    use Symfony\Component\HttpFoundation\Request;

    public function newAction(Request $request)
    {
        // just setup a fresh $task object (remove the dummy data)
        $task = new Task();

        $form = $this->createFormBuilder($task)
            ->add('task', TextType::class)
            ->add('dueDate', DateType::class)
            ->add('save', SubmitType::class, array('label' => 'Create Task'))
            ->getForm();

        $form->handleRequest($request);

        if ($form->isSubmitted() && $form->isValid()) {
            // ... perform some action, such as saving the task to the database

            return $this->redirectToRoute('task_success');
        }

        return $this->render('default/new.html.twig', array(
            'form' => $form->createView(),
        ));
    }

.. caution::

    Należy pamiętać, że metoda ``createView()`` powinna być wywoływana przed tym,
    jak wywołana jest ``handleRequest``. W przeciwnym razie, zmiany dokonane w
    zdarzeniach ``*_SUBMIT`` nie beda zastosowane do widoku (takie jak błędy
    walidacyjne).

   
.. versionadded:: 2.3
    W Symfony 2.3 została dodana metoda :method:`Symfony\Component\Form\FormInterface::handleRequest`.
    Poprzednio ``$request`` był przekazywany do metody ``submit`` - strategia, która jest już
    przestarzała i zostanie usunięta w Symfony 3.0. W celu poznania szczegółów tej metody proszę
    zapoznać się z :ref:`cookbook-form-submit-request`.

W tej akcji akceptuje się powszechny wzorzec obsługi formularzy, co ma trzy możliwe tryby:

#. Podczas początkowego ładowania strony w przeglądarce, formularz jest tworzony
   i renderowany. Metoda :method:`Symfony\Component\Form\FormInterface::handleRequest`
   uznaje, że formularz nie został zgłoszony i nie robi nic. Jeśli formularz nie
   został zgłoszony, metoda :method:`Symfony\Component\Form\FormInterface::isValid`
   zwraca ``false``;

#. Gdy użytkownik zgłosi formularza, metoda
   :method:`Symfony\Component\Form\FormInterface::handleRequest` rozpoznaje to i
   natychmiast zapisuje zgłoszone dane z powrotem do właściwości ``task`` i ``dueDate``
   obiektu ``$task``. Następnie obiekt ten jest sprawdzany. Jeśli jest nieprawidłowy
   (walidacja jest omówiona w dalszym rozdziale), metoda
   :method:`Symfony\Component\Form\FormInterface::isValid` zwraca ``false``,
   więc formularz jest znowu renderowany razem ale ze wszystkimi błędami wykazanymi
   w walidacji;
   
   .. note::

       Można użyć metodę :method:`Symfony\Component\Form\FormInterface::isSubmitted`
       aby sprawdzić, czy formularz został zgłoszony, bez względu na to, czy zgłoszone
       dane są rzeczywiście prawidłowe, czy nie.

#. Gdy użytkownik zgłosi formularz z prawidłowymi danymi, zgłoszone dane są z powrotem
   zapisywane do formularza, ale tym razem metoda
   :method:`Symfony\Component\Form\FormInterface::isValid` zwraca ``true``.
   Teraz programista ma okazję aby wykonać kilka akcji, używając obiektu ``$task``
   (np. utrwalić dane w bazie danych), przed przekierowaniem użytkownika do innej
   strony (np. do strony "Dziękujemy").
   
   .. note::
      
      Przekierowanie użytkownika po udanym zgłoszeniu formularza uniemożliwia
      użytkownikowi, by odświeżył i ponownie przesłał dane.   

.. index::
   single: formularze; wiele przycisków zgłaszających

.. _book-form-submitting-multiple-buttons:

Zgłaszanie formularzy z wieloma przyciskami
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. versionadded:: 2.3
    W Symfony 2.3 dodano obsługę przycisków w formularzach.

Gdy formularz zawiera więcej niż jeden przycisk zgłaszający, to zachodzi potrzeba
informacji o tym, który przycisk został kliknięty, aby dostosować kod
w akcji. Dodajmy do naszego formularza drugi przycisk z etykietą
"Zapisz i dodaj"::

    $form = $this->createFormBuilder($task)
        ->add('task', TextType::class)
        ->add('dueDate', DateType::class)
        ->add('save', SubmitType::class, array('label' => 'Create Task'))
        ->add('saveAndAdd', SubmitType::class, array('label' => 'Save and Add'))
        ->getForm();

W akcji zastosujemy metodę przycisku
:method:`Symfony\\Component\\Form\\ClickableInterface::isClicked`
w celu zapytania, czy kliknięty został przycisk "Zapisz i dodaj"::

    if ($form->isValid()) {
        // wykonanie jakiegoś działania, jak zapis zadania do bazy danych

        $nextAction = $form->get('saveAndAdd')->isClicked()
            ? 'task_new'
            : 'task_success';

        return $this->redirect($this->generateUrl($nextAction));
    }

.. index::
   single: formularze; walidacja
   
.. _book-forms-form-validation:

Walidacja formularza
--------------------

W poprzednim rozdziale dowiedzieliśmy się w jaki sposób można wysłać formularz
z poprawnymi i niepoprawnymi danymi. W Symfony walidacja jest stosowana do
wewnętrznego obiektu (np. ``Task``). Innymi słowami, nie chodzi o to czy
"formularz" jest prawidłowy, ale o to czy prawidłowy jest obiekt ``$task``,
po tym jak formularz zastosował na nim zgłoszone dane. Wywołanie ``$form->isValid()``
jest skrótem, który odpytuje obiekt ``$task`` o poprawność danych.

Walidacja realizowana jest poprzez dodanie do klasy zestawu zasad (zwanych
ograniczeniami, *ang. constraints*). Aby zobaczyć jak to działa, dodamy ograniczenia
walidacyjne, tak aby pole ``task`` nie mogło być puste a pole ``dueDate`` nie mogło
być puste i musiało by być prawidłowym obiektem ``DateTime``.

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        // AppBundle/Entity/Task.php
        use Symfony\Component\Validator\Constraints as Assert;

        class Task
        {
            /**
             * @Assert\NotBlank()
             */
            public $task;

            /**
             * @Assert\NotBlank()
             * @Assert\Type("\DateTime")
             */
            protected $dueDate;
        }

    .. code-block:: yaml
       :linenos:

        # AppBundle/Resources/config/validation.yml
        AppBundle\Entity\Task:
            properties:
                task:
                    - NotBlank: ~
                dueDate:
                    - NotBlank: ~
                    - Type: \DateTime

    .. code-block:: xml
       :linenos:

        <!-- AppBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping
                http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="AppBundle\Entity\Task">
                <property name="task">
                    <constraint name="NotBlank" />
                </property>
                <property name="dueDate">
                    <constraint name="NotBlank" />
                    <constraint name="Type">\DateTime</constraint>
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php
       :linenos:

        // AppBundle/Entity/Task.php
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\NotBlank;
        use Symfony\Component\Validator\Constraints\Type;

        class Task
        {
            // ...

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('task', new NotBlank());

                $metadata->addPropertyConstraint('dueDate', new NotBlank());
                $metadata->addPropertyConstraint(
                    'dueDate',
                    new Type('\DateTime')
                );
            }
        }


Doskonale! Jeśli ponownie zgłosimy formularz z nieprawidłowymi danymi, to zobaczymy
w formularzu odpowiednio wydrukowane komunikaty o błędach.

.. _book-forms-html5-validation-disable:

.. sidebar:: Walidacja HTML5

   Wraz z HTML5, wiele przeglądarek może natywnie egzekwować pewne ograniczenia
   walidacji po stronie klienta. Najczęściej walidacja jest aktywowana przez
   wygenerowanie wymaganego atrybutu w polu, które tego wymaga. W przeglądarkach
   obsługujących HTML5 skutkuje to natywnym wyświetleniem przez przeglądarkę
   komunikatu o błędzie, jeśli dokonywana jest próba wysłania formularza z pustym polem.
   
   Renderowane formularze w pełni korzystają z tej nowej funkcjonalności, przez
   dodanie stosownych atrybutów HTML wyzwalających walidację. Walidacja po stronie
   klienta może jednak zostać wyłączona przez dodanie atrybutu ``novalidate``
   w znaczniku ``form`` lub ``formnovalidate`` dla znacznika przycisku zgłaszającego
   (*ang. submit button*). Jest to szczególnie przydatne, gdy chce się przetestować
   ograniczenia po stronie serwera, ale zablokowanych w przeglądarce (umożliwiając
   zgłaszanie przez nią pustych pól).
   
   .. configuration-block::

       .. code-block:: html+twig

           {# app/Resources/views/default/new.html.twig #}
           {{ form(form, {'attr': {'novalidate': 'novalidate'}}) }}

       .. code-block:: html+php

           <!-- app/Resources/views/default/new.html.php -->
           <?php echo $view['form']->form($form, array(
               'attr' => array('novalidate' => 'novalidate'),
           )) ?>

Walidacja jest bardzo zaawansowaną funkcjonalnością Symfony i opisana jest
w rozdziale :doc:`/book/validation`.

.. index::
   single: formularze; grupy walidacyjne

.. _book-forms-validation-groups:

Grupy walidacyjne
~~~~~~~~~~~~~~~~~

Jeżeli stosuje się :ref:`grupy walidacyjne <book-validation-validation-groups>`
w obiekcie, to należy określić, które grupy (grupa) mają być użyte::

    $form = $this->createFormBuilder($users, array(
        'validation_groups' => array('registration'),
    ))->add(...);

.. versionadded:: 2.7
    Metoda ``configureOptions()`` została wprowadzona w Symfony 2.7. Poprzednio,
    stosowana była metoda ``setDefaultOptions()``.

Jeżeli tworzy się :ref:`klasy formularza <book-form-creating-form-classes>`
(dobra praktyka), to potrzeba dodać następujący kod do metody ``configureOptions()``::

    use Symfony\Component\OptionsResolver\OptionsResolver;

    public function configureOptions(OptionsResolver $resolver)
    {
        $resolver->setDefaults(array(
            'validation_groups' => array('registration'),
        ));
    }
    

W obu przypadkach, do walidacji wewnętrznego obiektu zostaną użyte tylko
zarejestrowane grupy walidacyjne.

.. index::
   single: formularze; wyłączanie walidacji

Wyłączanie walidacji
~~~~~~~~~~~~~~~~~~~~

.. versionadded:: 2.3
    W Symfony 2.3 została dodana możliwość ustawienia opcji ``validation_groups``
    na ``false``, chociaż ustawienie w tej opcji pustej tablicy da ten sam wynik w
    poprzednich wersjach Symfony.

Czasami jest to przydatne w celu całkowitego powstrzymania walidacji formularza.
W takim przypadku można pominąć wywołanie w akcji metody
:method:`Symfony\\Component\\Form\\FormInterface::isValid`.
Jeśli nie jest to wskazane (z powodów niżej wymienionych) , to można ewentualnie
ustawić opcję ``validation_groups`` na ``false`` lub pustą tablicę::

    use Symfony\Component\OptionsResolver\OptionsResolver;

    public function configureOptions(OptionsResolver $resolver)
    {
        $resolver->setDefaults(array(
            'validation_groups' => false,
        ));
    }

Proszę zauważyć, że gdy się to zrobi, formularz w dalszym ciągu będzie uruchamiał
podstawowe sprawdzanie spójności, na przykład, czy przesyłany plik nie jest za
duży lub czy zgłoszone zostały nie istniejące pola. Jeśli chce sie wyłączyć walidacje,
można użyć :ref:`zdarzenia POST_SUBMIT <cookbook-dynamic-form-modification-suppressing-form-validation>`.

.. index::
   single: formularze; grupy walidacyjne


Grupy walidacyjne oparte na zgłoszonych danych
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jeśli zachodzi potrzeba stworzenia jakiejś zaawansowanej logiki dla określenia
grup walidacyjnych (np. w oparciu o zgłaszane dane), to można ustawić opcję
``validation_groups`` na wywołanie zwrotne tablicy::

    use Symfony\Component\OptionsResolver\OptionsResolver;

    // ...
    public function configureOptions(OptionsResolver $resolver)
    {
        $resolver->setDefaults(array(
            'validation_groups' => array(
                'AppBundle\Entity\Client',
                'determineValidationGroups',
            ),
        ));
    }

Wywoła to statyczną metodę ``determineValidationGroups()`` klasy ``Client``
po powiązaniu formularza, ale przed wykonaniem walidacji. Obiekt formularza jest
przekazywany do tej metody jako argument (patrz następny przykład). Można również
określić całą logikę inline przy użyciu ``Closure``::

    use AppBundle\Entity\Client;
    use Symfony\Component\Form\FormInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    // ...
    public function configureOptions(OptionsResolver $resolver)
    {
        $resolver->setDefaults(array(
            'validation_groups' => function (FormInterface $form) {
                $data = $form->getData();

                if (Client::TYPE_PERSON == $data->getType()) {
                    return array('person');
                }

                return array('company');
            },
        ));
    }

Użycie opcji ``validation_groups`` przesłania używaną domyślną wartość grupy
walidacyjnej. Jeśli chce się stosować w walidacji domyślne ograniczenia jakiejś
encji, trzeba ustawić tą opcję następująco::

    use AppBundle\Entity\Client;
    use Symfony\Component\Form\FormInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    // ...
    public function configureOptions(OptionsResolver $resolver)
    {
        $resolver->setDefaults(array(
            'validation_groups' => function (FormInterface $form) {
                $data = $form->getData();

                if (Client::TYPE_PERSON == $data->getType()) {
                    return array('Default', 'person');
                }

                return array('Default', 'company');
            },
        ));
    }

Więcej informacji o działaniu grup walidacyjnych i domyślnych ograniczeń znajdziesz
w rozdziale podrecznika :ref:`"Grupy walidacyjne" <book-validation-validation-groups>`.


.. index::
   single: formularze; grupy walidacyjne

Grupy walidacyjne oparte na kliknietym przycisku
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. versionadded:: 2.3
    W Symfony 2.3 została dodana obsługa przycisków.

Gdy formularz zawiera wiele przycisków zgłaszających (*submit*), można zmienić
grupę walidacyjną w zależności od tego, który przycisk został użyty do zgłoszenia
formularza. Na przykład, rozpatrzmy formularz kreatora, który pozwala na przechodzenie
do następnego kroku lub na cofnięcie się do poprzedniego kroku. Załóżmy również,
że po cofnięciu się do poprzedniego kroku dane formularza trzeba zapisać ale bez walidacji.

Najpierw potrzebujemy dodać do formularza dwa przyciski::

    $form = $this->createFormBuilder($task)
        // ...
        ->add('nextStep', SubmitType::class)
        ->add('previousStep', SubmitType::class)
        ->getForm();

Następnie skonfigurujemy przycisk powrotu do poprzedniego kroku aby uruchamiał
określoną grupe walidacyjną. W naszym przykładzie chcemy, aby walidacja została
wyłączona, tak więc ustawiamy opcję ``validation_groups`` na ``false``::

    $form = $this->createFormBuilder($task)
        // ...
         ->add('previousStep', SubmitType::class, array(
            'validation_groups' => false,
        ))
        ->getForm();

Teraz formularz będzie pomijać swoje ograniczenia walidacyjne. Będą jeszcze sprawdzane
ograniczenia integralności, takie jak sprawdzenie czy przesyłany plik nie jest za
duży lub czy zgłoszono tekst w polu numerycznym.


.. index::
   single: formularze; wbudowane typy pól

.. _book-forms-type-reference:

Wbudowane typy pól
------------------

Symfony jest standardowo wyposażony w dużą grupę typów pól, która obejmuje wszystkie
popularne pola formularza i typy danych, z jakimi można mieć do czynienia:


.. include:: /reference/forms/types/map.rst.inc

Można również stworzyć własne typy pól. Temat ten jest omówiony w artykule
":doc:`Jak utworzyc własny typ pola formularza</cookbook/form/create_custom_field_type>`".

.. index::
   single: formularze; opcje typu pola

Opcje typu pola
~~~~~~~~~~~~~~~

Każdy typ pole ma kilka opcji, jakie mogą zostać użyte do skonfigurowania pola.
Na przykład, pole ``thedueDate`` jest obecnie renderowane jako 3 pola wyboru.
Jednak :doc:`DateType </reference/forms/types/date>` może zostać skonfigurowane,
tak aby były renderowane jako pojedyncze pola tekstowe (gdzie użytkownik wpisuje
datę jako ciąg znaków)::

    ->add('dueDate', DateType::class, array('widget' => 'single_text'))

.. image:: /images/book/form-simple2.png
    :align: center

Każdy typ pola ma kilka różnych opcji, które mogą zostać do nich przekazane.
Wiele z nich jest określana dla danego typu pola. Szczegółowe informacje znajdziesz
w dokumentacji każdego typu pola.

.. sidebar:: Opcja ``required``

    Najbardziej popularną opcja jest ``required``, która może być zastosowana do
    każdego pola. Domyślnie opcja ``required`` jest ustawiona na ``true``, co oznacza,
    że przeglądarki obsługujace HTML5 będą stosowały walidację po stronie klienta,
    zgłaszając błąd, jeśli pole jest puste. Jeśli nie chcesz takiego zachowania,
    to ustaw opcję ``required`` na ``false`` lub wyłącz
    :ref:`walidację HTML5<book-forms-html5-validation-disable>`.

    Należy również pamiętać, że ustawienie opcji ``required`` na ``true`` **nie da
    rezultatu** przy walidacji po stronie serwera. Innymi słowy, jeśli użytkownik wyśle
    pustą wartość dla tego pola (albo przykładowo posługuje się starą przeglądarką
    lub serwisem internetowym), to zostanie to zaakceptowane jako wartość prawidłowa,
    chyba że zostały użyte ograniczenia ``NotBlank`` lub ``NotNull``.

    Mówiąc inaczej, opcja ``required`` jest "super", ale prawdziwą walidację po
    stronie serwera należy stosować zawsze.

.. sidebar:: Opcja ``label``

    Etykietę dla pola formularza można ustawić opcją ``label``, którą można
    zastosować do każdego pola::

        ->add('dueDate', DateType::class, array(
            'widget' => 'single_text',
            'label'  => 'Due Date',
        ))

    Etykietę pola można również ustawić w szablonie generującym formularz
    – patrz niżej. Jeśli nie potrzebujesz etykiety powiązanej z polem wejściowym,
    możesz to wyłączyć, ustawiając ta wartość na ``false``.

.. index::
   single: formularze; odgadywany typ pola

.. _book-forms-field-guessing:

Odgadywany typ pola
-------------------

Teraz, gdy zostały dodane metadane walidacyjne do klasy ``Task``, Symfony wie już
trochę o swoich polach. Jeśli zezwoli się, to Symfony może "odgadywać" typ pola
i ustawiać odgadnięty typ. W tym przykładzie Symfony może odgadywać typ pola z
zasad walidacyjnych, takich że zarówno pole ``task`` jest zwykłym polem ``TextType``,
jak i pole ``dueDate`` jest polem ``DateType``::

    public function newAction()
    {
        $task = new Task();

        $form = $this->createFormBuilder($task)
            ->add('task')
            ->add('dueDate', null, array('widget' => 'single_text'))
            ->add('save', SubmitType::class)
            ->getForm();
    }

"Odgadywanie" jest aktywowane gdy pominięty zostanie drugi argument metody ``add()``
(lub jeżeli przekaże się do niego wartość ``null``). Gdy przekaże się tablicę opcji
jako trzeci argument (w powyższym przykładzie zrobione jest to dla ``dueDate``),
opcje te zostaną zastosowane do pola o typie odgadywanym.

.. caution::

    Jeśli w formularzu używa się określonej grupy walidacyjnej, odgadywany typ
    pola będzie podczas odgadywania typu nadal uwzględniał wszystkie ograniczenia
    walidacyjne (w tym ograniczenia, które nie są częścią zastosowanych grup
    walidacyjnych).

.. index::
   single: formularze; odgadywany typ pola

Opcje odgadywanego typu pola
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Podczas odgadywania typu pola, Symfony może również próbować odgadnąć prawidłowe
wartości kilku opcji pola.

.. tip::

    Gdy te opcje są ustawione, pole będzie renderowane ze specjalnymi atrybutami
    HTML, które dostarczają walidację HTML5 po stronie serwera. Jednak to nie generuje
    równoważnych ograniczeń po stronie serwera (np. ``Assert\Length``). Chociaż trzeba
    będzie ręcznie dodać walidację po stronie serwera, te opcje typu pola można następnie
    odgadnąć na podstawie tej informacji.

``required``
   Opcja ``required`` może zostać odgadnięta w oparciu o zasady walidacyjne
   (tzn. jest to opcja ``NotBlank`` lub ``NotNull``) lub o metadane Doctrine
   (tzn. jest to opcja ``nullable``). Jest to bardzo przydatne, bo walidacja po
   stronie serwera będzie automatycznie dopasowana do zasad walidacji.

``max_length``
   Jeżeli pole jest jakimś polem tekstowym, to opcja ``max_length`` może zostać odgadnięta
   z ograniczeń walidacyjnych (jeżeli są użyte ``Length`` lub ``Range``) lub z metadanych
   Doctrine (poprzez długość pól).

.. note::

  Te opcje pola odgadywane są tylko gdy ustawi się Symfony do odgadywania typu pola
  (tj. gdy pominie się lub przekaże wartość ``null`` jako drugi argument ``add()``).

Jeśli chce się zmienić jedną z odgadywanych wartości, to można zastąpić ją przez
przekazanie opcji w tablicy opcji pola::

    ->add('task', null, array('attr' => array('maxlength' => 4)))

.. index::
   single: formularze; renderowanie w szablonie

.. _form-rendering-template:

Renderowanie formularza w szablonie
-----------------------------------

Dotąd widzieliśmy jak cały formularz może być renderowany z użyciem tylko jednej
linii kodu. Oczywiście zwykle potrzeba o wiele więcej elastyczności przy renderowaniu
formularza:

.. configuration-block::

    .. code-block:: html+twig
       :linenos:

        {# app/Resources/views/default/new.html.twig #}
        {{ form_start(form) }}
            {{ form_errors(form) }}

            {{ form_row(form.task) }}
            {{ form_row(form.dueDate) }}
        {{ form_end(form) }}

    .. code-block:: html+php
       :linenos:

        <!-- app/Resources/views/default/newAction.html.php -->
        <?php echo $view['form']->start($form) ?>
            <?php echo $view['form']->errors($form) ?>

            <?php echo $view['form']->row($form['task']) ?>
            <?php echo $view['form']->row($form['dueDate']) ?>
        <?php echo $view['form']->end($form) ?>

Znasz już funkcje ``form_start()`` i ``form_end()``, ale co robią pozostałe
funkcje?

``form_errors(form)``
   Renderuje wszystkie błędy wspólne dla całego formularza
   (błędy specyficzne dla pól są wyświetlane następnie przy każdym polu);

``form_row(form.dueDate)``
   Renderuje etykietę, wszystkie błędy i widget formularza
   HTML dla danego pola (np. ``dueDate``), domyślnie wewnątrz bloku ``div``;


Większość prac jest wykonywana przez funkcję pomocniczą ``form_row``, która renderuje
etykietę, błędy i widżet HTML formularza dla każdego pola wewnątrz znacznika,
domyślnie ``div``. W rozdziale ":ref:`form-theming`" dowiesz się jak wyjście ``form_row``
może zostać dopasowane na różnych poziomach.

.. tip::

    Do bieżących danych formularza można uzyskać dostęp poprzez ``form.vars.value``:

    .. configuration-block::

        .. code-block:: twig

            {{ form.vars.value.task }}

        .. code-block:: html+php

            <?php echo $form->vars['value']->getTask() ?>

.. index::
   single: formularze; ręczne renderowanie pól

Ręczne renderowanie każdego pola
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Funkcja pomocnicza ``form_row`` jest mocna ponieważ może bardzo szybko renderować
każde pole formularza (i można w niej również dostosować znacznik używany w "wierszu").
Istnieje też możliwość ręcznego ustawiania każdego pola z osobna. Dane wyjściowe
poniższej prezentowanego kodu są takie same, jak dla funkcji ``form_row``:

.. configuration-block::

    .. code-block:: html+twig
       :linenos:

        {{ form_start(form) }}
            {{ form_errors(form) }}

            <div>
                {{ form_label(form.task) }}
                {{ form_errors(form.task) }}
                {{ form_widget(form.task) }}
            </div>

            <div>
                {{ form_label(form.dueDate) }}
                {{ form_errors(form.dueDate) }}
                {{ form_widget(form.dueDate) }}
            </div>

            <div>
                {{ form_widget(form.save) }}
            </div>

        {{ form_end(form) }}

    .. code-block:: html+php
       :linenos:

        <?php echo $view['form']->start($form) ?>

            <?php echo $view['form']->errors($form) ?>

            <div>
                <?php echo $view['form']->label($form['task']) ?>
                <?php echo $view['form']->errors($form['task']) ?>
                <?php echo $view['form']->widget($form['task']) ?>
            </div>

            <div>
                <?php echo $view['form']->label($form['dueDate']) ?>
                <?php echo $view['form']->errors($form['dueDate']) ?>
                <?php echo $view['form']->widget($form['dueDate']) ?>
            </div>

            <div>
                <?php echo $view['form']->widget($form['save']) ?>
            </div>

        <?php echo $view['form']->end($form) ?>

Jeśli automatycznie generowana etykieta pola nie jest właściwa, to można określić
ją jawnie:

.. configuration-block::

    .. code-block:: html+twig

        {{ form_label(form.task, 'Task Description') }}

    .. code-block:: html+php

        <?php echo $view['form']->label($form['task'], 'Task Description') ?>

Niektóre typy pól mają dodatkowo renderowane opcje, które mogą być przekazane do
widżetu. Opcje te są udokumentowane w dokumentacji każdego typu, ale jedną
z popularnniejszych opcji jest ``attr``, która umożliwia modyfikację atrybutów w
elemencie formularza. Poniżej dodamy klasę ``task_field`` do renderowanego wejściowego
pola tekstowego:

.. configuration-block::

    .. code-block:: html+twig

        {{ form_widget(form.task, {'attr': {'class': 'task_field'}}) }}

    .. code-block:: html+php

        <?php echo $view['form']->widget($form['task'], array(
            'attr' => array('class' => 'task_field'),
        )) ?>
        
Jeśli musisz renderować pola formularza "ręcznie", to możesz uzyskać dostęp do
poszczególnych wartości atrybutów pól, takich jak ``id``, ``name`` i ``label``.
Na przykład, aby pobrać wartość ``id``:

.. configuration-block::

    .. code-block:: html+twig

        {{ form.task.vars.id }}

    .. code-block:: html+php

        <?php echo $form['task']->vars['id']?>


Aby pobrać wartość używaną w atrybucie nazwy pola formularza potrzeba użyć wartości
``full_name``:

.. configuration-block::

    .. code-block:: html+twig

        {{ form.task.vars.full_name }}

    .. code-block:: html+php

        <?php echo $form['task']->vars['full_name'] ?>

Informacja o funkcjach szablonowych Twig
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jeśli używasz Twig, to pełna informacja o funkcjach renderujących formularz jest
dostępna w :doc:`Reference Manual </reference/forms/twig_reference>`. Przeczytaj
to aby dowiedzieć się wszystkiego o dostępnych funkcjach pomocniczych i opcjach
jakie można użyć w każdej z nich.

.. index::
   single: Forms; Changing the action and method

.. _book-forms-changing-action-and-method:

Zmiana akcji i metody HTTP formularza
-------------------------------------

Dotychczas funkcja pomocnicza ``form_start()`` była używana do renderowania
początkowego znacznika formularza i zakładaliśmy, że każdy formularz jest
zgłaszany na ten sam adres co w żądaniu POST. Czasem zachodzi potrzeba zmiany
tych parametrów. Można to zrobić na kilka różnych sposobów. Jeżeli buduje się
formularz w kontrolerze, to można użyć metod ``setAction()`` i ``setMethod()``::

    $form = $this->createFormBuilder($task)
        ->setAction($this->generateUrl('target_route'))
        ->setMethod('GET')
        ->add('task', TextType::class)
        ->add('dueDate', DateType::class)
        ->add('save', SubmitType::class)
        ->getForm();

.. note::

    W tym przykładzie zakłada się, że utworzono trasę o nazwie ``target_route``,
    która wskazuje na akcję przetwarzającą formularz.

W rozdziale :ref:`book-form-creating-form-classes` można dowiedzieć się, jak
przekształcić kod tworzący formularz na oddzielne klasy. W przypadku używania
zewnętrznej klasy formularza, można przekazać akcję i metodę jako
opcję formularza::

    use AppBundle\Form\Type\TaskType;
    // ...

    $form = $this->createForm(TaskType::class, $task, array(
        'action' => $this->generateUrl('target_route'),
        'method' => 'GET',
    ));

Wreszcie można zastąpić akcję i metodę w szablonie przekazując je do funkcji
pomocniczych ``form()`` lub ``form_start()``:

.. configuration-block::

    .. code-block:: html+twig

        {# app/Resources/views/default/new.html.twig #}
        {{ form_start(form, {'action': path('target_route'), 'method': 'GET'}) }}

    .. code-block:: html+php

        <!-- app/Resources/views/default/newAction.html.php -->
        <?php echo $view['form']->start($form, array(
            // The path() method was introduced in Symfony 2.8. Prior to 2.8,
            // you had to use generate().
            'action' => $view['router']->path('target_route'),
            'method' => 'GET',
        )) ?>

.. note::

    Jeśli metodą formularza nie jest GET lub POST, ale PUT, PATCH lub DELETE, to
    Symfony wstawia ukryte pole z nazwą "_method", które przechowuje nazwę tej
    metody. Taki formularz będzie zgłaszany w zwykłym żądaniu POST, ale router
    Symfony jest w stanie wykryć parametr "_method" i zinterpretuje żądanie jako
    PUT, PATCH lub DELETE. Proszę zapoznać się z artykułem
    ":doc:`/cookbook/routing/method_parameters`" w celu uzyskania więcej informacji.
    

.. index::
   single: formularze; tworzenie klas

.. _book-form-creating-form-classes:

Tworzenie klas formularza
-------------------------

Jak widać, formularz może być utworzony i używany bezpośrednio w kontrolerze.
Jednak lepszym rozwiązaniem jest zbudowanie formularza w oddzielnej, samodzielnej
klasie, która może być następnie używana wiele razy w różnych miejscach aplikacji.
Utwórzmy nową klasę, która będzie miejscem logiki dla zbudowania formularza zadania::

    // src/AppBundle/Form/Type/TaskType.php
    namespace AppBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\Form\Extension\Core\Type\SubmitType;

    class TaskType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('task')
                ->add('dueDate', null, array('widget' => 'single_text'))
                ->add('save', SubmitType::class)
            ;
        }
    }

Nowa klasa zawiera wszystkie wskazówki potrzebne do utworzenia formularza naszego
zadania, co można wykorzystać do szybkiego zbudowania obiektu formularza w akcji::

    // src/AppBundle/Controller/DefaultController.php
    use AppBundle\Form\Type\TaskType;

    public function newAction()
    {
        $task = ...;
        $form = $this->createForm(TaskType::class, $task);

        // ...
    }

Umieszczenie logiki formularza we własnej klasie oznacza, że formularz może być
łatwo ponownie wykorzystany w jakimkolwiek miejscu projektu. Jest to najlepszy
sposób na tworzenie formularzy, ale wybór zależy tylko od Ciebie.

.. _book-forms-data-class:

.. sidebar:: Ustawienie ``data_class``

    Każdy formularz musi znać nazwę klasy, która przechowuje wewnetrzne dane
    (np. ``AppBundle\Entity\Task``). Zazwyczaj jest ona odgadywana w oparciu
    o nazwę obiektu przekazywanego jako drugi argument metody ``createForm``
    (tj. obiektu klasy ``$task``). Później, gdy zaczniesz osadzanie formularza,
    nie będzie to wystarczające. Tak więc, choć nie zawsze jest to wystarczające,
    dobrym pomysłem jest jawnie określić opcję ``data_class`` przez dodanie
    następującego kodu do klasy typu formularza::

        use Symfony\Component\OptionsResolver\OptionsResolver;

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults(array(
                'data_class' => 'AppBundle\Entity\Task',
            ));
        }

.. tip::

    Kiedy odwzorowuje się formularze na obiekty, to z założenia odwzorowywane są
    wszystkie pola. Jakiekolwiek pola formularza, które nie istnieją w odwzorowywanym
    obiekcie, spowodują zrzucenie wyjątku.

    W przypadku gdy w formularzu potrzebne są dodatkowe pola (przykładowo, pole
    wyboru "Czy zgadzasz się z tymi warunkami?"), które nie będą odwzorowywane
    do wewnętrznego obiektu, potrzeba ustawić opcję ``mapped`` na ``false``::

        use Symfony\Component\Form\FormBuilderInterface;

        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('task')
                ->add('dueDate', null, array('mapped' => false))
                ->add('save', SubmitType::class)
            ;
        }

    Dodatkowo, jeśli są jakieś pola w formularzu, które nie zostały dołączone
    w przesłanych danych, to pola te zostaną jawnie ustawione na null.

    Dane pola mogą być dostępne w akcji przez::

        $form->get('dueDate')->getData();
        
    Ponadto, dane w nieodwzorowanym polu mogą być również modyfikowane bezpośrednio::

        $form->get('dueDate')->setData(new \DateTime());
    
Definiowanie formularzy jako usługi
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Typ formularza może mieć pewne zewnętrzne zależności. Można zdefiniować własny
typ formularza jako usługę i wstrzyknąć do niego wszystkie potrzebne zależności.

.. note::

    Usługi i kontener usług zostanę omówione
    :doc:`dalszym rozdziale </book/service_container>` i rzecz stanie się
    bardziej zrozumiała po jego przeczytaniu.

Przyjmijmy, że chcemy w swoim typie formularza użyć usługę zdefiniowaną jako
``app.my_service``. Utwórzmy konstruktor dla naszego typu formularza, w celu
uzyskania usługi::

    // src/AppBundle/Form/Type/TaskType.php
    namespace AppBundle\Form\Type;

    use App\Utility\MyService;
    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\Form\Extension\Core\Type\SubmitType;

    class TaskType extends AbstractType
    {
        private $myService;

        public function __construct(MyService $myService)
        {
            $this->myService = $myService;
        }

        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            // You can now use myService.
            $builder
                ->add('task')
                ->add('dueDate', null, array('widget' => 'single_text'))
                ->add('save', SubmitType::class)
            ;
        }
    }

Następnie zdefiniujmy typ formularza jako usługę.

.. configuration-block::

    .. code-block:: yaml
       :linenos:
       
       # src/AppBundle/Resources/config/services.yml
        services:
            app.form.type.task:
                class: AppBundle\Form\Type\TaskType
                arguments: ["@app.my_service"]
                tags:
                    - { name: form.type } 
         
    .. code-block:: xml
       :linenos:

        <!-- src/AppBundle/Resources/config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="app.form.type.task" class="AppBundle\Form\Type\TaskType">
                    <tag name="form.type" />
                    <argument type="service" id="app.my_service"></argument>
                </service>
            </services>
        </container>

    .. code-block:: php
       :linenos:

        // src/AppBundle/Resources/config/services.php
        use Symfony\Component\DependencyInjection\Reference;

        $container->register('app.form.type.task', 'AppBundle\Form\Type\TaskType')
            ->addArgument(new Reference('app.my_service'))
            ->addTag('form.type')

Proszę przeczytać :ref:`form-cookbook-form-field-service` w celu uzyskania więcej informacji.

.. index::
   pair: formularze; Doctrine
  
Formularze a Doctrine
---------------------

Celem formularza jest przetłumaczenie danych z obiektu (np. ``Task``) na formularz
HTML i przetłumaczenie przesłanych z powrotem przez użytkownika danych na oryginalny
obiekt. Jako taki, temat utrwalania obiektu ``Task`` w bazie danych jest całkowicie
niezależny od tematu formularza. Trzeba tu jednak zaznaczyć, że jeśli ma się
skonfigurowaną klasę ``Task`` to aby została ona utrwalana poprzez Doctrine
(czyli po dodaniu :ref:`metadanych odwzorowania <book-doctrine-adding-mapping>`),
to utrwalenie jej po zgłoszeniu formularza może zostać zrealizowane, gdy formularz
jest prawidłowy::

    if ($form->isValid()) {
        $em = $this->getDoctrine()->getManager();
        $em->persist($task);
        $em->flush();

        return $this->redirectToRoute('task_success');
        
    }

Jeśli z jakiegoś powodu nie ma się dostępu do oryginalnego obiektu ``$task``,
można pobrać go z formularza::

    $task = $form->getData();

Więcej informacji znajdziesz w rozdziale :doc:`doctrine`.

Kluczowe jest zrozumienie, że gdy formularz zostaje wysłany, to przesłane dane
są natychmiast transferowane do wewnętrznego obiektu. Jeśli chce się utrwalać te
dane, to po prostu trzeba utrwalić sam obiekt (który zawiera już przesłane dane).

.. index::
   single: formularze; formularze osadzone

Formularze osadzone
-------------------

Często zachodzi potrzeba zbudowania formularza zawierającego pola z różnych obiektów.
Na przykład, formularz rejestracji może zawierać dane należące do obiektu ``User``
jak również do wielu obiektów klasy ``Address``.
Na szczęście, jest to łatwe i naturalne z użyciem komponentu formularza.

Osadzanie pojedyńczych obiektów
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Załóżmy, że każdy obiekt ``Task`` należy do prostego obiektu ``Category``.
Rozpoczniemy oczywiście od utworzenia obiektu ``Category``::

    // src/AppBundle/Entity/Category.php
    namespace AppBundle\Entity;

    use Symfony\Component\Validator\Constraints as Assert;

    class Category
    {
        /**
         * @Assert\NotBlank()
         */
        public $name;
    }

Następnie dodamy nową właściwość ``category`` do klasy ``Task``::

    // ...

    class Task
    {
        // ...

        /**
         * @Assert\Type(type="AppBundle\Entity\Category")
         * @Assert\Valid()
         */
        protected $category;

        // ...

        public function getCategory()
        {
            return $this->category;
        }

        public function setCategory(Category $category = null)
        {
            $this->category = $category;
        }
    }

.. tip::

    Do właściwości ``category`` zostało dodane ograniczenie ``Valid``. Kaskaduje ono
    walidację w odpowiedniej encji. Jeśli pominie się to ograniczenie, encje potomne
    nie będa walidowane.

Teraz, gdy aplikacja została zaktualizowana w celu odzwierciedlenia nowych wymagań,
utworzymy klasę formularza, tak aby obiekt ``Category`` mógł być modyfikowany przez
użytkownika::

    // src/AppBundle/Form/Type/CategoryType.php
    namespace AppBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    class CategoryType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('name');
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults(array(
                'data_class' => 'AppBundle\Entity\Category',
            ));
        }
    }

Ostatecznym celem jest umożliwienie, aby kategorie zadań mogły być modyfikowane
wewnątrz formularza zadania. W tym celu dodamy pole ``category`` do obiektu
``TaskType``, którego typem jest instancja nowej klasy ``CategoryType``:

.. code-block:: php
   :linenos:   
      
    use Symfony\Component\Form\FormBuilderInterface;
    use AppBundle\Form\Type\CategoryType;

    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        // ...

        $builder->add('category', CategoryType::class);
    }

Pola z ``CategoryType`` mogą teraz być renderowane obok pól z ``TaskType``.

Wyrenderujmy pola ``Category`` w ten sam sposób jak miało to miejsce w przypadku
pól ``Task``:

.. configuration-block::

    .. code-block:: html+twig
       :linenos:

        {# ... #}

        <h3>Category</h3>
        <div class="category">
            {{ form_row(form.category.name) }}
        </div>

        {# ... #}

    .. code-block:: html+php
       :linenos:

        <!-- ... -->

        <h3>Category</h3>
        <div class="category">
            <?php echo $view['form']->row($form['category']['name']) ?>
        </div>

        <!-- ... -->
    
Gdy użytkownik zgłasza formularz, przesłane dane dla pól ``Category`` są używane
do konstruowania instancji ``Category``, która jest ustawiona na pole ``category``
instacji ``Task``.

Instancja ``Category`` jest dostępna naturalnie poprzez ``$task->getCategory()``
i może być utrwalona w bazie danych lub gdzie się tego potrzebuje.

Osadzanie kolekcji formularzy
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Można również osadzić kolekcję formularzy w jednym formularzu (wyobraź sobie
formularz ``Category`` z wieloma sub-formularzami ``Product``). Wykonuje się to
przez użycie typu pola ``collection``.

Więcej informacji można uzyskać w artykule
":doc:`Jak osadzoć kolekcję fotmularzy </cookbook/form/form_collections>` oraz
w referencjach :doc:`CollectionType </reference/forms/types/collection>`.

.. index::
   single: formularze; dekorowanie
   single: formularze; dostosowywanie pól

.. _form-theming:

Dekorowanie formularza
----------------------

Każda część renderowanego formularza może być dostosowywana. Ma się możliwość
ustalania sposobu renderowania każdego „wiersza”, zmiany znacznika używanego do
wyświetlania błędów, a nawet dostosowania tego, jak powinien być renderowany
znacznik ``textarea``. Nie ma istotnych ograniczeń i można zastosować różne zmiany
w różnych miejscach.

Symfony używa szablonów do renderowania każdej części formularza, takiej jak
znaczniki ``label``, znaczniki ``input`` itd.

W Twig każdy "fragment" formularza jest reprezentowany przez blok Twiga.
Aby dostosować jakąś część formularza, wystarczy zastąpić określony blok

W PHP każdy "fragment" formularza jest renderowany przez indywidualny plik szablonowy.
Dla dostosowania jakiejś części formularza potrzeba zastąpić istniejący szablon nowym.

Aby zrozumieć jak to działa, dostosujemy fragment ``form_row`` i dodamy atrybut
``class`` do elementu ``div``, który otacza każdy wiersz. Aby to osiągnąć, utworzymy
nowy plik szablonowy, który zawierać będzie nowy znacznik:

.. configuration-block::

    .. code-block:: html+twig
       :linenos:

        {# app/Resources/views/form/fields.html.twig #}
        {%- block form_row -%}
         <div class="form_row">
                {{- form_label(form) -}}
                {{- form_errors(form) -}}
                {{- form_widget(form) -}}
         </div>
        {% endblock form_row %}

    .. code-block:: html+php
       :linenos:

        <!-- app/Resources/views/form/form_row.html.php -->
        <div class="form_row">
            <?php echo $view['form']->label($form, $label) ?>
            <?php echo $view['form']->errors($form) ?>
            <?php echo $view['form']->widget($form, $parameters) ?>
        </div>


Fragment formularza ``form_row`` jest używany podczas renderowania większości pól
poprzez funkcje ``form_row``. W celu powiadomienia komponentu formularza, by używał
wyżej określony nowy fragment ``form_row``, trzeba dodać następujący kod w górnej
części szablonu:

.. configuration-block::

    .. code-block:: html+twig
       :linenos:

        {# app/Resources/views/default/new.html.twig #}
        {% form_theme form 'form/fields.html.twig' %}

        {# or if you want to use multiple themes #}
        {% form_theme form 'form/fields.html.twig' 'form/fields2.html.twig' %}

        {# ... render the form #}

    .. code-block:: html+php
       :linenos:

        <!-- app/Resources/views/default/new.html.php -->
        <?php $view['form']->setTheme($form, array('form')) ?>

        <!-- or if you want to use multiple themes -->
        <?php $view['form']->setTheme($form, array('form', 'form2')) ?>

        <!-- ... render the form -->


Znacznik ``form_theme`` (w Twig) "importuje" fragmenty kodu zdefiniowane w danym
szablonie i używa ich podczas renderowania formularza. Innymi słowami, gdy później
w szablonie wywoływana jest funkcja ``form_row``, to zostanie użyty blok ``form_row``
z własnego motywu (zamiast domyślnego bloku ``form_row``, który dostarczany jest
w Symfony).

Niestandardowy motyw nie musi zastępować wszystkich bloków. Blok, który nie został
zastąpiony podczas renderowania we własnej skórce, będzie pobrany z motywu globalnego
(zdefiniowanej na poziomie pakietu).

Jeśli stosuje się kilka niestandardowych motywów, to będą one wyszukiwane w kolejności
bloków podanych w motywie globalnym.

W celu dostosowania jakiegoś fragment formularza, zachodzi potrzeba zastąpienia
tego fragmentu. Jest to omówione w rozdziale następnym.

Więcej informacji można znaleźć w dokumencie
:doc:`Jak dostosować renderowany formularz</cookbook/form/form_customization>`.

.. index::
   single: formularze; nazewnictwo fragmentów formularza

.. _form-template-blocks:

Nazewnictwo fragmentów formularza
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

W Symfony, każda renderowana część formularza (elementy HTML formularza, etykiety itd.)
jest definiowana w szablonie podstawowym, który jest w Twig kolekcją bloków i kolekcją
plików szablonowych w PHP.

W Twig, każdy potrzebny blok jest zdefiniowany w pojedyńczym pliku szablonowym
(`form_div_layout.html.twig`_), który znajduje się wewnątrz `Twig Bridge`_.
Wewnątrz tego pliku, można zobaczyć każdy blok potrzebny do wygenerowania formularza
i każdego domyślnego typu pola.

W PHP, fragmenty są indywidualnymi plikami szablonowymi. Domyślnie są one umieszczone
w katalogu ``Resources/views/Form`` pakietu FrameworkBundle ( `zobacz to na GitHub`_).

Każda nazwa fragmentu stosuje ten sam podstawowy wzorzec i składa się z dwóch części,
rozdzielonych znakiem podkreślenia (``_``). Oto kilka przykładów:

* ``form_row`` - używana przez ``form_row`` do renderowania większości pól;
* ``textarea_widget`` - używana przez ``form_widget`` do renderowania pól typu *textarea*;
* ``form_errors`` - używana przez ``form_errors`` do renderowania komunikatów błędów przy polach;

Każdy fragment stosuje nazwę według tego samego wzorca: *typ_fragment*.
Część ``typ`` odpowiada renderowanemu typowi pola (np. *textarea*, *checkbox*,
*date* itd.), natomiast część ``fragment`` odnosi się do rodzaju renderowanego
elementu formularza (np. *label*, *widget*, *errors* itd.). Domyślnie możliwe
są 4 fragmenty formularza (określane w części ``fragment``, które mogą być renderowane:

+------------+-----------------------+-----------------------------------------------------------+
| ``label``  | (np. ``form_label``)  | renderuje etykiety pola                                   |
+------------+-----------------------+-----------------------------------------------------------+
| ``widget`` | (np. ``form_widget``) | renderuje reprezentację pól HTML                          |
+------------+-----------------------+-----------------------------------------------------------+
| ``errors`` | (np. ``form_errors``) | renderuje komunikaty błędów pól                           |
+------------+-----------------------+-----------------------------------------------------------+
| ``row``    | (np. ``form_row``)    | renderuje cały wiersz formularza (label, widget i errors) |
+------------+-----------------------+-----------------------------------------------------------+

.. note::

    Istnieją jeszcze 2 inne części - ``rows`` i ``rest``, ale bardzo
    rzadko się je używa, jeżeli w ogóle.

Znając typ pola (np. *textarea*) i część formularza, którą chce się dostosować
(np. *widget*), można skonstruować nazwę fragmentu, który ma być przesłonięty
(tj. ``textarea_widget``).

.. index::
   single: formularze; dziedziczenie fragmentów formularza

Dziedziczenie fragmentów formularza
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

W niektórych przypadkach fragment, który chce się dostosować, może być wykazywany
jako zaginiony. Na przykład, w domyślnych szablonach dostarczany przez Symfony nie
występują fragmenty ``textarea_errors``. Tak więc jak są renderowane komunikaty
błędów dla pola *textarea*?

Odpowiedź brzmi: poprzez fragment ``form_errors``. Kiedy Symfony renderuje komunikaty
błędów dla typu *textarea*, to najpierw szuka fragmentu ``textarea_errors`` zanim
wykorzysta fragment ``form_errors``. Każdy typ pola ma swój typ nadrzędny (nadrzędny
typ *textarea* to *text*, a tego z kolei to *form*) i Symfony używa fragmentu dla
typu nadrzędnego jeśli fragment podstawowy nie istnieje.

Tak więc, aby przesłonić komunikaty błędów tylko dla pól *textarea*, trzeba skopiować
fragment ``form_errors``, zmienić jego nazwę na ``textarea_errors`` i odpowiednio
dostosować. Aby zastąpić domyślnie renderowane komunikaty błędów dla wszystkich pól,
trzeba skopiować i dostosować bezpośrednio fragment ``form_errors``.

.. tip::

    "Nadrzędny" typ każdego typu pola jest opisany w
    :doc:`informacji o typach pól </reference/forms/types>` opisanej dla każdego
    typu pola.

.. index::
   single: formularze; dekorowanie globalne formularza

Dekorowanie formularza w zakresie globalnym
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

W powyższym przykładzie użyliśmy ``form_theme helper`` (w Twig) do "zaimportowania"
dostosowanych fragmentów formularza do tego formularza. Można również powiadomić
Symfony aby importował przeróbki formularza w całym projekcie.

.. _book-forms-theming-global:

Twig
....

Aby automatycznie dołączyć własne bloki, z utworzonego wcześniej szablonu
``fields.html.twig``, do wszystkich szablonów, trzeba przerobić plik konfiguracyjny
aplikacji:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        twig:
            form_themes:
                - 'form/fields.html.twig'
            # ...

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/twig http://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <twig:config>
                <twig:theme>form/fields.html.twig</twig:theme>
                <!-- ... -->
            </twig:config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('twig', array(
            'form_themes' => array(
                'form/fields.html.twig',
            ),
            // ...
        ));

Teraz wszystkie bloki wewnątrz szablonu ``fields.html.twig`` są udostępniane
globalnie przy określaniu wyjścia formularza.

.. sidebar::  Dostosowywanie całego wyjścia formularza w jednym pliku w Twig

    W Twig można również dostosować blok formularza bezpośrednio w szablonie,
    gdy takie dostosowanie jest potrzebne:

    .. code-block:: html+twig
       :linenos:  

        {% extends 'base.html.twig' %}

        {# import "_self" as the form theme #}
        {% form_theme form _self %}

        {# make the form fragment customization #}
        {% block form_row %}
            {# custom field row output #}
        {% endblock form_row %}

        {% block content %}
            {# ... #}

            {{ form_row(form.task) }}
        {% endblock %}

    Znacznik ``{% form_theme form _self %}`` umożliwia dostosowywanie bloków
    formularza bezpośrednio w szablonie, który będzie używał tych dostosowań.
    Używaj tej metody do szybkiego dostosowania wyjścia formularza, które
    będzie tylko czasem potrzebne w pojedynczym szablonie.

    .. caution::

        Funkcjonalność ``{% form_theme form _self %}`` działa tylko jeśli szablon
        rozszerza inny szablon. Jeżeli takiego szablonu nie ma, to należy wskazać
        form_theme do innego szablonu.

PHP
...

Aby automatycznie dołączyć własne szablony z wcześniej utworzonego katalogu
``app/Resources/views/Form`` do wszystkich szablonów, trzeba poprawić
plik konfiguracyjny aplikacji:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        framework:
            templating:
                form:
                    resources:
                        - 'Form'
        # ...

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:templating>
                    <framework:form>
                        <framework:resource>Form</framework:resource>
                    </framework:form>
                </framework:templating>
                <!-- ... -->
            </framework:config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'templating' => array(
                'form' => array(
                    'resources' => array(
                        'Form',
                    ),
                ),
            ),
            // ...
        ));

Teraz wszystkie fragmenty wewnątrz katalogu ``app/Resources/views/Form``
są dostępne globalnie przy określaniu wyjścia formularza.

.. index::
   single: formularze; ochrona przed CSRF

.. _forms-csrf:

Ochrona przed CSRF
------------------

CSRF (`Cross-Site Request Forgery`_), zwane też *fałszywym wywołaniem międzywitrynowym*,
to forma ataku, polegająca na tym, że nieświadomy użytkownik wykonuje żądanie
spreparowane przez osobę atakującą. Na szczęście ataki CSRF mogą być zablokowane
przez użycie w formularzach tokenów.

Dobrą wiadomością jest to, że domyślnie Symfony automatycznie osadza i sprawdza
tokeny CSRF. Oznacza to, że można skorzystać z ochrony przed CSRF nie robiąc nic.
W rzeczywistości każdy formularz użyty w tym rozdziale korzystał z tokenu
zabezpieczającego przed CSRF.

Ochrona przed CSRF działa w ten sposób, że do formularza dodawane jest ukryte pole,
o domyślnej nazwie ``_token``, którego wartość znana jest tylko serwerowi i klientowi.
To gwarantuje, że użytkownik (a nie jakiś inny podmiot) jest uprawniony do przesłania
danych formularza. Symfony automatycznie sprawdza obecność i rzetelność tego tokenu.

Pole ``_token`` jest ukrytym polem i zostanie automatycznie wygenerowane, jeśli
dołączy się w szablonie funkcję ``form_end()``. Funkcja ta zapewnia, że na wyjściu
znajdują się wszystkie nie renderowane pola.

.. caution::

    Ponieważ token jest przechowywany w sesji a sesja jest rozpoczynana
    automatycznie jak tylko formularz zostanie wygenerowany, to jest on od razu
    chroniony przed atakami CSRF.

Token CSRF może zostać dopasowany w konfiguracji formularza. Przykładowo::

    use Symfony\Component\OptionsResolver\OptionsResolver;

    class TaskType extends AbstractType
    {
        // ...

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults(array(
                'data_class'      => 'AppBundle\Entity\Task',
                'csrf_protection' => true,
                'csrf_field_name' => '_token',
                // a unique key to help generate the secret token
                'csrf_token_id'   => 'task_item',
            ));
        }

        // ...
    }
    
.. _form-disable-csrf:

Aby wyłączyć ochronę przed CSRF, trzeba ustawić opcję ``csrf_protection`` na ``false``.
Dostosowanie może być również wykonane globalnie dla całego projektu. Więcej informacji
znajdziesz w rozdziale :ref:`Informacje o konfiguracji formularza<reference-framework-form>`.

.. note::

    Opcja ``csrf_token_id`` jest opcjonalna, ale znacznie zwieksza bezpieczeństwo
    generowanego tokenu, przez jego indywidualizację.

.. versionadded:: 2.4
    Opcja ``csrf_token_id`` została wprowadzona w Symfony 2.4. Wcześniej, trzeba
    było wykorzystywać opcję ``intention``.
    

.. caution::

    Tokeny CSRF mają być różne dla każdego użytkownika. Dlatego też trzeba być
    ostrożnym, jeśli próbuje się buforować strony z formularzami zawierającymi ten
    rodzaj ochrony. Wiecej informacji znajdziesz w artykule
    :doc:`/cookbook/cache/form_csrf_caching`.



.. index::
   single: formularze; bez klasy

Stosowania formularza bez użycia klasy
--------------------------------------

W większości przypadków formularz jest związany z obiektem a pola formularza
pobierają i przechowują swoje dane we właściwościach obiektu. Jest to dokładnie
to, co widzieliśmy do tej pory w tej części podręcznika.

Ale czasami możemy chcieć stosować formularz bez klasy i zwracać tablicę zgłoszonych
danych. Jest to w rzeczywistości bardzo proste::

    // make sure you've imported the Request namespace above the class
    use Symfony\Component\HttpFoundation\Request;
    // ...

    public function contactAction(Request $request)
    {
        $defaultData = array('message' => 'Type your message here');
        $form = $this->createFormBuilder($defaultData)
            ->add('name', TextType::class)
            ->add('email', EmailType::class)
            ->add('message', TextareaType::class)
            ->add('send', SubmitType::class)
            ->getForm();

        $form->handleRequest($request);

        if ($form->isValid()) {
            // data is an array with "name", "email", and "message" keys
            $data = $form->getData();
        }

        // ... render the form
    }

Domyślnie, formularz w rzeczywistości zakłada, że chce się pracować z tablicą danych
zamiast z obiektem. Istnieją dwa sposoby zmiany tego zachowania i związania formularza
z obiektem: 

#. Przekazanie obiekt podczas tworzenia formularza (jako pierwszy argument metody
   ``createFormBuilder`` lub jako drugi argument metody ``createForm``);

#. Zadeklarowanie opcji ``data_class`` w formularzu.


Jeśli nie zrobi się którejś z powyższych czynności, to formularz zwróci dane jako
tablicę. W tym przykładzie, ponieważ ``$defaultData`` nie jest obiektem (i nie jest
ustawiona opcja ``data_class``), to ``$form->getData()`` ostatecznie zwraca tablicę.

.. tip::

    Można również uzyskać bezpośredni dostęp do wartości POST (w tym przypadku "name")
    za pomocą obiektu żądania, podobnie do tego::

        $request->request->get('name');

    Należy pamiętać, że w większości przypadków lepszym wyborem jest użycie metody
    ``getData()``, ponieważ zwracane są dane (zazwyczaj obiekt) po tym jak zostały
    one przetworzone przez komponent Form.

Dodawanie walidacji
~~~~~~~~~~~~~~~~~~~

Jedynym, nie omówionym jeszcze, elementem jest walidacja. Zazwyczaj, gdy wywołuje się
metodę ``$form->isValid()``, to obiekt zostaje sprawdzony przez renderowanie
ograniczenia, które zastosowało się dla tej klasy. Jeśli formularz jest odwzorowany
na obiekt (np. przez użycie opcji ``data_class`` lub przekazanie obiektu do formularza),
jest to podejście najlepsze. Zobacz :doc:`validation`
w celu poznania szczegółów.

.. _form-option-constraints:

Ale jeśli nie odwzoruje się formularza na obiekt i zamiast tego chce się pobrać
prostą tablice zgłoszonych danych, to jak można dodać ograniczenia dla danych formularza?

Rozwiązaniem jest ustawienie sobie ograniczeń i dołączenie ich do indywidualnych pól.
Ogólne podejście jest omówione trochę szerzej w rozdziale :ref:`Walidacja<book-validation-raw-values>`,
ale oto krótki przykład:

.. code-block:: php
   :linenos:   

    use Symfony\Component\Validator\Constraints\Length;
    use Symfony\Component\Validator\Constraints\NotBlank;
    use Symfony\Component\Form\Extension\Core\Type\TextType;

    $builder
       ->add('firstName', TextType::class, array(
           'constraints' => new Length(array('min' => 3)),
       ))
       ->add('lastName', TextType::class, array(
           'constraints' => array(
               new NotBlank(),
               new Length(array('min' => 3)),
           ),
       ))
    ;

.. tip::

    Jeśli używa się grup walidacyjnych, to zachodzi potrzeba albo odwoływania się
    do ``Defaultgroup`` podczas tworzenia formularza lub ustawienia właściwej grupy
    na dodawanym ograniczeniu.
    
.. code-block:: php
   
    new NotBlank(array('groups' => array('create', 'update'))
    
    
Wnioski końcowe
---------------

Teraz już poznałeś wszystkie klocki niezbędne do budowy złożonych i funkcjonalnych
formularzy w swojej aplikacji. Podczas budowania formularza należy pamiętać, że
pierwszym celem formularza jest przetłumaczenie danych z obiektu formularza (``Task``)
na formularz HTML, tak aby użytkownik mógł modyfikować dane. Drugim celem formularza
jest przejęcie danych przesłanych przez użytkownikai ponownego naniesienie ich na
obiekt.

Jest jeszcze przed Tobą wiele nauki o nieomówionych tu zagadnieniach z zakresu
formularzy, takich jak :doc:`obsługa ładowania plików w Doctrine </cookbook/doctrine/file_uploads>`
czy jak tworzyć formularz, w którym może być dodawana dynamicznie jakaś liczba
sub-formularzy (np. lista zadań do wykonania, gdzie można udostępnić dodawanie pól
poprzez Javascript przed wysłaniem danych). Przeczytaj artykuły o tym zagadnieniu
w Receptariuszu. Ponadto trzeba by poznać :doc:`dokumentację typów pól </reference/forms/types>`,
która zawiera przykłady używania typów pól i ich opcji.

Dalsza lektura
--------------

* :doc:`/cookbook/doctrine/file_uploads`
* :doc:`FileType Reference </reference/forms/types/file>`
* :doc:`Creating Custom Field Types </cookbook/form/create_custom_field_type>`
* :doc:`/cookbook/form/form_customization`
* :doc:`/cookbook/form/dynamic_form_modification`
* :doc:`/cookbook/form/data_transformers`
* :doc:`/cookbook/security/csrf_in_login_form`
* :doc:`/cookbook/cache/form_csrf_caching`


.. _`Komponent Form`: https://github.com/symfony/Form
.. _`DateTime`: http://php.net/manual/en/class.datetime.php
.. _`Twig Bridge`: https://github.com/symfony/symfony/tree/2.2/src/Symfony/Bridge/Twig
.. _`form_div_layout.html.twig`: https://github.com/symfony/symfony/blob/2.2/src/Symfony/Bridge/Twig/Resources/views/Form/form_div_layout.html.twig
.. _`Cross-site request forgery`: http://en.wikipedia.org/wiki/Cross-site_request_forgery
.. _`zobacz na GitHub`: https://github.com/symfony/symfony/tree/2.2/src/Symfony/Bundle/FrameworkBundle/Resources/views/Form
.. _`2.8 UPGRADE Log`: https://github.com/symfony/symfony/blob/2.8/UPGRADE-2.8.md#form
