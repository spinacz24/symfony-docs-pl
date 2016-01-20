.. highlight:: php
   :linenothreshold: 2

.. index::
   double: formularze; najlepsze praktyki

Formularze
==========

Formularze są komponentem Symfony, przy stosowaniu których popełniane
są często błędy, co może wynikać z rozległego zakresu i dużej ilość
funkcjonalności. W tym rozdziale omawiamy kilka z najlepszych praktyk, umożliwiających
pełne wykorzystanie możliwości formularzy ale też szybkie wykonanie pracy.

Budowanie formularzy
--------------------

.. best-practice::

    Zdefiniuj swój formularz jako klasę PHP.

Komponent Form umożliwia budowanie formularzy bezpośrednio w kodzie kontrolera.
Jest to bardzo dobre rozwiązanie, jeśli nie ma potrzeby ponownego uzycia formularza
gdzieś indziej. W celu właściwej organizacji kodu i umożliwienia wielokrotnego
wykorzystywania formularzy, zaleca sie definiowanie poszczególnych formularzy
w ich własnych klasach PHP::

    namespace AppBundle\Form;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    class PostType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('title')
                ->add('summary', TextareaType::class)
                ->add('content', TextareaType::class)
                ->add('authorEmail', EmailType::class)
                ->add('publishedAt', DateTimeType::class)
            ;
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults(array(
                'data_class' => 'AppBundle\Entity\Post'
            ));
        }
    }

.. best-practice::

    Wstawiaj klasy typu form w przestrzeni nazewniczej ``AppBundle\Form``,
    chyba że, używasz innych klas formularzowych, jak na przykład transformery
    danych.

W celu użycia klasy, trzeba zastosować metodę ``createForm`` i przekazać w pełni
kwalifikowaną nazwę klasy::

    // ...
    use AppBundle\Form\PostType;

    // ...
    public function newAction(Request $request)
    {
        $post = new Post();
        $form = $this->createForm(PostType::class, $post);

        // ...
    }

Rejestrowanie formularzy jako usługi
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Można również
:ref:`zarejestrować typ form jako usługę <form-cookbook-form-field-service>`.
Nie jest to jednak zalecane, chyba że planuje się wielokrotne wykorzystywanie
nowego typu form w wielu miejscach aplikacji lub chce się osadzać go w innym
formularzu bezpośrednio albo za pomocą
:doc:`CollectionType </reference/forms/types/collection>`.

Dla większości formularzy, które są uzywane do edycji lub tworzenia czegoś,
rejestrowanie formularza jako usługi jest nadmiernym obciążeniem i powoduje,
że trudniej jest odczytać w kontrolerze, która klasę wykorzystuje formularz.

Konfiguracja przycisku formularza
---------------------------------

Klasy formularzy powinny być obojętne na to, gdzie będa wykorzystywane. Ułatwia
to ich późniejsze wykorzystywanie w wielu miejscach.

.. best-practice::

    Przyciski dodawaj w szablonach, nie w klasach formularzy, czy w kontrolerach.

Począwszy od Symfony 2.3 można w formularzu dodawać przyciski jako pola formularza.
Jest to znaczne uproszczenie pracy nad szablonem renderujacym formularz.
Jeśli doda się przyciski bezpośrednio w klasie formularza, to skutecznie ograniczy
się zakres stosowania takiego formularza:

.. code-block:: php

    class PostType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                // ...
               ->add('save', SubmitType::class, array('label' => 'Create Post'))
            ;
        }

        // ...
    }

Ten formularz może być zaprojektowany do tworzenia wpisów, ale jeśli będzie się
chciało go ponownie wykorzystać do edytowania wpisów, to etykieta przycisku będzie
zła. Zamiast tego, niektórzy programiści konfoguruja przyciski formularza w kontrolerze::

    namespace AppBundle\Controller\Admin;

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Symfony\Component\Form\Extension\Core\Type\SubmitType;
    use AppBundle\Entity\Post;
    use AppBundle\Form\PostType;

    class PostController extends Controller
    {
        // ...

        public function newAction(Request $request)
        {
            $post = new Post();
            $form = $this->createForm(PostType::class, $post);
            $form->add('submit', SubmitType::class, array(
                'label' => 'Create',
                'attr'  => array('class' => 'btn btn-default pull-right')
            ));

            // ...
        }
    }

Jest to też duży błąd, ponieważ mieszaa prezentację znaczników (etykiet, klas
CSS itd.) z czystym kodem PHP. Rozdzielanie tych rzeczy jest zawsze dobrą praktyką
i wszystkie elementy odnoszące się do widoku należy umieszczać w warstwie widoku:

.. code-block:: html+twig
   :linenos:

    {{ form_start(form) }}
        {{ form_widget(form) }}

        <input type="submit" value="Create"
               class="btn btn-default pull-right" />
    {{ form_end(form) }}

Renderowanie formularzy
-----------------------

Istnieje wiele sposobów renderowania formularza, począwszy od umieszczenia całej
rzeczy w jednej linii kodu, aż po renderowanie każdego elementu formularza oddzielnie.
Najlepszy sposób zależy od potrzeby dostosowania wyglądu formularza.

Jednym z najprostszych sposobów, szczególnie przydatnych podczas programowania,
jest wyrenderowania znaczników formularza i użycie funkcji ``form_widget()`` do
wyrenderowania wszystkich pól i etykiet:

.. code-block:: html+twig
   :linenos:

    {{ form_start(form, {'attr': {'class': 'my-form-class'} }) }}
        {{ form_widget(form) }}
    {{ form_end(form) }}

Jeśli potrzeba większej kontroli nad sposobem renderowania pól formularza, to
powinno się usunąć funkcję ``form_widget(form)`` i renderować pola indywidualnie.
W celu uzyskania więcej informacji, proszę zapoznać się z artykułem
:doc:`/cookbook/form/form_customization`.

.. index::
   single: formularze, samozgłaszanie
   single: samozgłaszanie formularza

Obsługa zgłoszonego formularza
------------------------------

Obsługa zgłoszonego formularza jest zwykle realizowana w następujacy sposób:

.. code-block:: php
   :linenos:

    public function newAction(Request $request)
    {
        // build the form ...

        $form->handleRequest($request);

        if ($form->isSubmitted() && $form->isValid()) {
            $em = $this->getDoctrine()->getManager();
            $em->persist($post);
            $em->flush();

            return $this->redirect($this->generateUrl(
                'admin_post_show',
                array('id' => $post->getId())
            ));
        }

        // render the template
    }

Są tutaj dwie istotne kwestie. Pierwsza, to zalecenie aby stosować tą samą akcję
zarówno do renderowania formularza jak i obsługi zgłaszonego formularza (tzw.
*samozgłaszanie formularza*).
Na przykład, można mieć akcję ``newAction``, która renderuje tylko formularz oraz
oddzielna akcję ``createAction``, która tylko przetwarza zgłoszony formularz.
Obydwie akcje będą niemal identyczne. Prościej jest więc zastosować tylko ``newAction``
do obsługi wszystkiego.

Po drugie, dla jasności, zalecamy stosowanie ``$form->isSubmitted()`` w wyrażeniu
``if``. Nie jest to technicznie konieczne, ponieważ ``isValid()`` najpierw wywołuje
``isSubmitted()``. Jednak bez tego, przepływ przetwarzania nie czyta się dobrze,
wyglądając tak, jakby formularza był zawsze przetwarzany (nawet przy żądaniu GET).

