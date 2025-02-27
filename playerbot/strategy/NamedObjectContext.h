#pragma once
#include <cstdarg>
#include <string>
#include <iosfwd>

namespace ai
{
    using namespace std;

    class Qualified
    {
    public:
        Qualified() {};
        Qualified(string qualifier) : qualifier(qualifier) {}
        Qualified(int32 qualifier1) { Qualify(qualifier1); }
    public:
        virtual void Qualify(int32 qualifier) { ostringstream out; out << qualifier; this->qualifier = out.str(); }
        virtual void Qualify(string qualifier) { this->qualifier = qualifier; }
        string getQualifier() { return qualifier; }
        void Reset() { qualifier.clear(); }

        static string MultiQualify(vector<string> qualifiers, string separator)
        { 
            ostringstream out; 
            for (auto& qualifier : qualifiers)
            {
                out << qualifier << (&qualifier != &qualifiers.back() ? separator : "");
            }
                
            return out.str();
        }

        static vector<string> getMultiQualifiers(string qualifier1, string separator)
        { 
            vector<string> result;

            size_t last = 0; 
            size_t next = 0; 
            while ((next = qualifier1.find(separator, last)) != string::npos)
            { 
                result.push_back(qualifier1.substr(last, next - last));
                last = next + 1;
            } 

            result.push_back(qualifier1.substr(last));
            return result;
        }
        
        static int32 getMultiQualifierInt(string qualifier1, uint32 pos, string separator) { return stoi(getMultiQualifiers(qualifier1, separator)[pos]); }
        static string getMultiQualifierStr(string qualifier1, uint32 pos, string separator) { return getMultiQualifiers(qualifier1, separator)[pos]; }
    
    protected:
        string qualifier;
    };

    template <class T> class NamedObjectFactory
    {
    protected:
        typedef T* (*ActionCreator) (PlayerbotAI* ai);
        map<string, ActionCreator> creators;

    public:
        T* create(string name, PlayerbotAI* ai)
        {
            size_t found = name.find("::");
            string qualifier;
            if (found != string::npos)
            {
                qualifier = name.substr(found + 2);
                name = name.substr(0, found);
            }

            if (creators.find(name) == creators.end())
                return NULL;

            ActionCreator creator = creators[name];
            if (!creator)
                return NULL;

            T *object = (*creator)(ai);
            Qualified *q = dynamic_cast<Qualified *>(object);
            if (q && found != string::npos)
                q->Qualify(qualifier);

            return object;
        }

        set<string> supports()
        {
            set<string> keys;
            for (typename map<string, ActionCreator>::iterator it = creators.begin(); it != creators.end(); it++)
                keys.insert(it->first);
            return keys;
        }
    };


    template <class T> class NamedObjectContext : public NamedObjectFactory<T>
    {
    public:
        NamedObjectContext(bool shared = false, bool supportsSiblings = false) :
            NamedObjectFactory<T>(), shared(shared), supportsSiblings(supportsSiblings) {}

        T* create(string name, PlayerbotAI* ai)
        {
            if (created.find(name) == created.end())
                return created[name] = NamedObjectFactory<T>::create(name, ai);

            return created[name];
        }

        virtual ~NamedObjectContext()
        {
            Clear();
        }

        void Clear()
        {
            for (typename map<string, T*>::iterator i = created.begin(); i != created.end(); i++)
            {
                if (i->second)
                    delete i->second;
            }

            created.clear();
        }

        void Update()
        {
            for (typename map<string, T*>::iterator i = created.begin(); i != created.end(); i++)
            {
                if (i->second)
                    i->second->Update();
            }
        }

        void Reset()
        {
            for (typename map<string, T*>::iterator i = created.begin(); i != created.end(); i++)
            {
                if (i->second)
                    i->second->Reset();
            }
        }

        bool IsShared() { return shared; }
        bool IsSupportsSiblings() { return supportsSiblings; }

        set<string> GetCreated()
        {
            set<string> keys;
            for (typename map<string, T*>::iterator it = created.begin(); it != created.end(); it++)
                keys.insert(it->first);
            return keys;
        }

    protected:
        map<string, T*> created;
        bool shared;
        bool supportsSiblings;
    };

    template <class T> class NamedObjectContextList
    {
    public:
        virtual ~NamedObjectContextList()
        {
            for (typename list<NamedObjectContext<T>*>::iterator i = contexts.begin(); i != contexts.end(); i++)
            {
                NamedObjectContext<T>* context = *i;
                if (!context->IsShared())
                    delete context;
            }
        }

        void Add(NamedObjectContext<T>* context)
        {
            contexts.push_back(context);
        }

        T* GetObject(string name, PlayerbotAI* ai)
        {
            for (typename list<NamedObjectContext<T>*>::iterator i = contexts.begin(); i != contexts.end(); i++)
            {
                T* object = (*i)->create(name, ai);
                if (object) return object;
            }
            return NULL;
        }

        void Update()
        {
            for (typename list<NamedObjectContext<T>*>::iterator i = contexts.begin(); i != contexts.end(); i++)
            {
                if (!(*i)->IsShared())
                    (*i)->Update();
            }
        }

        void Reset()
        {
            for (typename list<NamedObjectContext<T>*>::iterator i = contexts.begin(); i != contexts.end(); i++)
            {
                (*i)->Reset();
            }
        }

        set<string> GetSiblings(string name)
        {
            for (typename list<NamedObjectContext<T>*>::iterator i = contexts.begin(); i != contexts.end(); i++)
            {
                if (!(*i)->IsSupportsSiblings())
                    continue;

                set<string> supported = (*i)->supports();
                set<string>::iterator found = supported.find(name);
                if (found == supported.end())
                    continue;

                supported.erase(found);
                return supported;
            }

            return set<string>();
        }

        set<string> supports()
        {
            set<string> result;

            for (typename list<NamedObjectContext<T>*>::iterator i = contexts.begin(); i != contexts.end(); i++)
            {
                set<string> supported = (*i)->supports();

                for (set<string>::iterator j = supported.begin(); j != supported.end(); j++)
                    result.insert(*j);
            }
            return result;
        }

        set<string> GetCreated()
        {
            set<string> result;

            for (typename list<NamedObjectContext<T>*>::iterator i = contexts.begin(); i != contexts.end(); i++)
            {
                set<string> createdKeys = (*i)->GetCreated();

                for (set<string>::iterator j = createdKeys.begin(); j != createdKeys.end(); j++)
                    result.insert(*j);
            }
            return result;
        }

    private:
        list<NamedObjectContext<T>*> contexts;
    };

    template <class T> class NamedObjectFactoryList
    {
    public:
        virtual ~NamedObjectFactoryList()
        {
            for (typename list<NamedObjectFactory<T>*>::iterator i = factories.begin(); i != factories.end(); i++)
                delete *i;
        }

        void Add(NamedObjectFactory<T>* context)
        {
            factories.push_front(context);
        }

        T* GetObject(string name, PlayerbotAI* ai)
        {
            for (typename list<NamedObjectFactory<T>*>::iterator i = factories.begin(); i != factories.end(); i++)
            {
                T* object = (*i)->create(name, ai);
                if (object) return object;
            }
            return NULL;
        }

    private:
        list<NamedObjectFactory<T>*> factories;
    };
};
