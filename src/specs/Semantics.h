#ifndef __SPECS__SEMANTICS_H__
#define __SPECS__SEMANTICS_H__

namespace CoQuiAAS {

    typedef enum {SEM_STABLE, SEM_COMPLETE, SEM_GROUNDED, SEM_PREFERRED, SEM_SEMISTABLE, SEM_STAGE, SEM_IDEAL, SEM_TRIATHLON, SEM_UNDEFINED} SemanticName;

    class Semantics {

        public:

        Semantics(SemanticName name, bool isDynamic) {
            this->name = name;
            this->isDyn = isDynamic;
        }

        inline SemanticName getName() {
            return this->name;
        }

        inline bool isDynamic() {
            return this->isDyn;
        }

        private:

        SemanticName name;

        bool isDyn;

    };
}

#endif