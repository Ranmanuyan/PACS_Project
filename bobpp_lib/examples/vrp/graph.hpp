

#ifndef BOBGRAPH
#define BOBGRAPH

/** \defgroup graphgroup Graph
 * @{
 */

typedef int cost_t;
typedef int dist_t;
typedef int load_t;
typedef int timwind_t;
typedef int coord_t;
typedef double redcost_t;


/** Constraint on vertex or edge
 */
template <class T>
class Constraint_info {
   protected :
   T tin;
   T tax;

   public:
   /** constructor
     */
   Constraint_info() : tin(0),tax(0) { }
   /** constructor
     */
   Constraint_info(const T &_ax) : tin(0),tax(_ax) { }
   /** constructor
     */
   Constraint_info(const Constraint_info<T> &c) : tin(c.tin),tax(c.tax) { }
   /** constructor
     */
   Constraint_info(const T &_in,const T &_ax) : tin(_in),tax(_ax) { }
   /** update the value
     */
   void update(const Constraint_info<T> &c) { 
      tin=(tin>c.tin?c.tin:tin);
      tax=(tax>c.tax?c.tax:tax);
   }
   const T &min() const { return tin; }
   const T &max() const { return tax; }
   bool isok(const T &t) const { return t>=tin && t<=tax; }
   /** get the size of the space state
     */
   T size() { tax-tin; }
   /** comparison
     */
   virtual void Pack(Bob::Serialize &bs) const {
         bs.Pack(&tin,1);
         bs.Pack(&tax,1);
   }
   virtual void UnPack(Bob::DeSerialize &bs) {
         bs.UnPack(&tin,1);
         bs.UnPack(&tax,1);
   }
};

template <class T>
class Constraint_value {
   protected :
   T t;
   public:
   Constraint_value() : t() { }
   Constraint_value(const T& _t) : t(_t) { }
   Constraint_value(const Constraint_value<T> &cv) : t(cv.t) { }
   void set(const T& _t) { t=_t; }
   const T &get() const { return t; }
   T &get() { return t; }
   virtual void Pack(Bob::Serialize &bs) const {
         Bob::Pack(bs,&t,1);
   }
   virtual void UnPack(Bob::DeSerialize &bs) {
         Bob::UnPack(bs,&t,1);
   }

};

template <class T,class C>
class Constraint_label {
   protected :
   T t;
   public:
   Constraint_label() : t() { }
   Constraint_label(const T& _t) : t(_t) { }
   Constraint_label(const Constraint_label<T,C> &cv) : t(cv.t) { }
   virtual int is_const_dom(const T &tp) const { return t<=tp;  }
   virtual int is_const_dom(const Constraint_label<T,C> &l) const { return is_const_dom(l.t);  }
   virtual int is_cost_dom(const C &mc,const T &_t,const C &c) const {
      if ( is_const_dom(_t) ) {
           if ( mc>c ) { return 1; }
           return -1;
      }
      if ( mc>c ) { return -1; }
      return 0;
   }
   virtual int is_cost_dom(const C &mc,const Constraint_label<T,C> &l,const C &c) const {
      return is_cost_dom(mc,l.t,c);
   }
   const T &get_index(const Constraint_info<T> &ci) { return t-ci.min(); }
   bool update(const Constraint_value<T> &cv,const Constraint_info<T> &ci) {
      t+=cv.get();
      return ci.isok(t); 
   }
};



/** Features Composers for 2 features
 */
template <class f1, class f2>
class Feature2 :public f1, f2 {
   public:
    Feature2() : f1(),f2() {}
    Feature2(const f1 &_f1, const f2 &_f2) : f1(_f1),f2(_f2) {}
    Feature2(const Feature2 &_f) : f1(_f),f2(_f) {}
    virtual ~Feature2() { }
    /** the virtual method to display an object on a ostream
      */
    virtual std::ostream &Prt(std::ostream &os) const {
         f1::Prt(os);
         f2::Prt(os);
         return os;
    }
    virtual void Pack(Bob::Serialize &bs) const {
         f1::Pack(bs);
         f2::Pack(bs);
    }
    virtual void UnPack(Bob::DeSerialize &bs) {
         f1::UnPack(bs);
         f2::UnPack(bs);
    }
};

/** Features Composers for 3 features
 */
template <class f1, class f2, class f3>
class Feature3 :public f1, f2, f3 {
   public:
    Feature3() : f1(),f2(),f3() {}
    Feature3(const f1 &_f1, const f2 &_f2,const f3 &_f3) : f1(_f1),f2(_f2),f3(_f3) {}
    Feature3(const Feature3 &_f) : f1(_f),f2(_f),f3(_f) {}
    virtual ~Feature3() { }
    /** the virtual method to display an object on a ostream
      */
    virtual std::ostream &Prt(std::ostream &os) const {
         f1::Prt(os);
         f2::Prt(os);
         f3::Prt(os);
         return os;
    }
    virtual void Pack(Bob::Serialize &bs) const {
         f1::Pack(bs);
         f2::Pack(bs);
         f3::Pack(bs);
    }
    virtual void UnPack(Bob::DeSerialize &bs) {
         f1::UnPack(bs);
         f2::UnPack(bs);
         f3::UnPack(bs);
    }
};

/** Features Composers for 4 features
 */
template <class f1, class f2, class f3, class f4>
class Feature4 :public f1, f2, f3 ,f4 {
   public:
    Feature4() : f1(),f2(),f3(),f4() {}
    Feature4(const f1 &_f1, const f2 &_f2,const f3 &_f3,const f4 &_f4) : f1(_f1),f2(_f2),f3(_f3),f4(_f4) {}
    Feature4(const Feature4 &_f) : f1(_f),f2(_f),f3(_f),f4(_f) {}
    virtual ~Feature4() { }
    /** the virtual method to display an object on a ostream
      */
    virtual std::ostream &Prt(std::ostream &os) const {
         f1::Prt(os);
         f2::Prt(os);
         f3::Prt(os);
         f4::Prt(os);
         return os;
    }
    virtual void Pack(Bob::Serialize &bs) const {
         f1::Pack(bs);
         f2::Pack(bs);
         f3::Pack(bs);
         f4::Pack(bs);
    }
    virtual void UnPack(Bob::DeSerialize &bs) {
         f1::UnPack(bs);
         f2::UnPack(bs);
         f3::UnPack(bs);
         f4::UnPack(bs);
    }
};

/** Class used to store a Graph.
 * This class is the base graph that only stores vertices and edge datas.
 */
template<class vi, class ei>
class Graph_Abs {

  public:
   /** Constructor
     */
   Graph_Abs()  { }
   /** Destructor
     */
   virtual ~Graph_Abs()  { }
   /** return the number of vertices.
     */
   virtual int size() const =0;
   /** resize the graphe
     */
   virtual void resize(int nv) =0;
   /** returns the vertice info
     * @param i the vertex id
     * @return the vertex info
     */
   virtual vi &get(int i) =0;
   /** returns the vertice info
     * @param i the vertex id
     * @return the vertex info
     */
   virtual const vi &get(int i) const =0;

   /** returns the edge info
     * @param i the source edge id
     * @param j the dest. edge id
     * @return the edge info
     */
   virtual const ei &get(int i,int j) const =0;
   /** returns the edge info
     * @param i the source edge id
     * @param j the dest. edge id
     * @return the edge info
     */
   virtual ei &get(int i,int j) =0;
   /** set an edge
     */
   virtual void set(int i,int j, const ei &e) =0;
   /** set an edge
     */
   virtual void set(const ei &e) =0;
   /** set an vertex
     */
   virtual void set(const vi &v) =0;
   
   virtual std::ostream &Prt(std::ostream &os) const {
      for (int i=0;i<size(); i++ ) {
         get(i).Prt(os);
         os << "==>";
         for (int j=0;j<size(); j++ ) {
           if ( get(i,j).isset() ) get(i,j).Prt(os);
         }
         os<< "\n";
      }
      return os;
   }
   
};


/** Class used to store a Graph.
 * This class is the base graph that only stores vertices and edge datas.
 */
template<class gi, class vi, class ei>
class Graph_comp : public Graph_Abs<vi,ei>, public gi {
  Bob::pvector<vi> vertices;
  Bob::pvector<Bob::pvector<ei> > edges;
  public:
   /** constructor
     * @param nv number of vertices
     */
   Graph_comp() : Graph_Abs<vi,ei>(), gi(), vertices(), edges() { }
   /** constructor
     * @param nv number of vertices
     */
   Graph_comp(const gi &_gi) : Graph_Abs<vi,ei>(), gi(_gi), vertices(), edges() { }
   /** constructor
     * @param nv number of vertices
     */
   Graph_comp(const gi &_gi,int nv) : Graph_Abs<vi,ei>(), gi(_gi),vertices(nv), edges(nv,Bob::pvector<ei>(nv)) { }
   /** constructor
     * @param nv number of vertices
     * @param v the default value for a vertex
     */
   Graph_comp(const gi &_gi,int nv,const vi &v) : Graph_Abs<vi,ei>(), gi(_gi),vertices(nv,v), edges(nv,Bob::pvector<ei>(nv)) { }
   /** constructor
     * @param nv number of vertices
     * @param v the default value for a vertex
     * @param e the default value for an edge
     */
   Graph_comp(const gi &_gi,int nv,const vi &v,const ei &e) : Graph_Abs<vi,ei>(), gi(_gi),vertices(nv,v), edges(nv,Bob::pvector<ei>(nv,e)) { }
   /** return the number of vertices.
     */
   int size() const { return vertices.size(); }
   /** resize the graphe
     */
   void resize(int nv) {
      std::cout << "resizegraph to "<< nv <<"\n";
      vertices.resize(nv);
      edges.resize(nv);
      for (int i=0;i<nv;i++ ) {
         edges[i].resize(nv);
      }
   }
   /** returns the vertice info
     * @param i the vertex id
     * @return the vertex info
     */
   vi &get(int i) { return vertices[i]; }
   /** returns the vertice info
     * @param i the vertex id
     * @return the vertex info
     */
   const vi &get(int i) const { return vertices[i]; }

   /** returns the edge info
     * @param i the source edge id
     * @param j the dest. edge id
     * @return the edge info
     */
   ei &get(int i,int j) { return edges[i][j]; }
   /** returns the edge info
     * @param i the source edge id
     * @param j the dest. edge id
     * @return the edge info
     */
   const ei &get(int i,int j) const { return edges[i][j]; }
   /** set an edge
     */
   void set(int i,int j, const ei &e) { edges[i][j]=e;}
   /** set an edge
     */
   void set(const ei &e) { edges[e.src()][e.dst()]=e;}
   /** set an vertex
     */
   void set(const vi &v) { vertices[v.get_id()]=v;}
   
   virtual std::ostream &Prt(std::ostream &os) const {
      gi::Prt(os);
      Graph_Abs<vi,ei>::Prt(os);
      return os;
   }
   
};

template<class gi,class vi, class ei>
inline std::ostream &operator<<(std::ostream &os,const Graph_comp<gi,vi,ei> &c) {
   return c.Prt(os);
}

/** This class is a graph class that allow to forbid the accesses
 * to the node or edge.
 */

template<class gi, class vi, class ei>
class Graph_forb :public Graph_Abs<vi,ei> {
   protected:
   Graph_comp<gi,vi,ei> *gc;
   Bob::pvector<int> fvo;
   Bob::pvector<int> fvi;
   Bob::pvector<Bob::bit_vector> fe;
   void testgc() {
      if ( gc==0 ) {
         std::cerr <<" No graph pointer in graph forb...\n";
      }
   }
   void test_out(int i) {
      if ( i!=0 )  {
         int c=0;
         for (int v=0;v<gc->size()-1;v++ ) {
            if ( !is_forb(i,v) ) c++;
         }
         if ( c== 1 ) {
            for (int v=0;v<gc->size()-1;v++ ) {
             if ( !is_forb(i,v) ) fvo[i]=v;
            }
         }
         if ( c==0 ) { std::cerr<< "Strange "<<i<<" has no more possible 'out' edge\n";}
      }
 
   }

   void test_in(int i) {
      if ( i!=(gc->size()-1) ) {
         int c=0;
         for (int v=0;v<gc->size()-1;v++ ) {
            if ( !is_forb(v,i) ) c++;
         }
         if ( c== 1 ) {
            for (int v=0;v<gc->size()-1;v++ ) {
             if ( !is_forb(v,i) ) fvi[i]=v;
            }
         }
         if ( c==0 ) { std::cerr<< "Strange "<<i<<" has no more possible 'in' edge\n";}
      }
 
   }
   public:
   /** Constructor
     */
   Graph_forb() : gc(0),fvo(),fvi(), fe() { }
   /** Constructor
     */
   Graph_forb(const Graph_comp<gi,vi,ei> *_gc) : gc((Graph_comp<gi,vi,ei> *)_gc),fvo(_gc->size()),fvi(_gc->size()),fe(_gc->size(),Bob::bit_vector(_gc->size())) { }
   /** Constructor
     */
   Graph_forb(const Graph_forb<gi,vi,ei> &gf) : gc(gf.gc),fvo(gf.fvo),fvi(gf.fvi),fe(gf.fe) { }
   /** Set the graph
     */
   void set(const Graph_comp<gi,vi,ei> *_gc) { 
      gc=(Graph_comp<gi,vi,ei> *)_gc; 
      if ( fvo.size()!=(unsigned int)gc->size() ) { resize(gc->size()); }
   }
   /** return the number of vertices.
     */
   int size() const { return gc->size(); }
   /** resize the graphe
     */
   void resize(int nv) {
      fvo.resize(nv);
      fvi.resize(nv);
      fe.resize(nv,Bob::bit_vector(nv));
   }
   /** Fixe the edge (i,j)
     */
   void fixe(int i,int j ) { 
      if ( i!=0 ) {
         for (int v=0;v<gc->size()-1;v++ ) {
            if ( v!=j ) forb(i,v);
            fvo[i]=j;
         }
      }
      if ( j!=(gc->size()-1) ) {
         for (int v=0;v<gc->size()-1;v++ ) {
            if ( v!=i ) forb(v,j);
            fvi[j]=i;
         }
      }
   }
   /** get the graph information.
     */
   const gi &get_gi() const { return *gc; }
   /** get the graph information.
     */
   gi &get_gi() { return *gc; }
   /** test if the vertex is free or not (not fixed).
     */
   bool is_free_in(int i) { return fvi[i]==-1; }
   /** test if the vertex is free or not (not fixed).
     */
   bool is_free_out(int i) { return fvo[i]==-1; }
   /** is_forb
     */
   int fixe_in(int i) { return fvi[i]; }
   /** is_forb
     */
   int fixe_out(int i) { return fvo[i]; }
   /** is_forb
     */
   void forb(int i,int j) { 
      fe[i].set(j); 
      test_in(j);
      test_out(i);
   }
   /** is_forb
     */
   bool is_forb(int i,int j) { return fe[i].test(j); }
   /** returns the vertice info
     * @param i the vertex id
     * @return the vertex info
     */
   vi &get(int i) { return gc->get(i); }
   /** returns the vertice info
     * @param i the vertex id
     * @return the vertex info
     */
   const vi &get(int i) const { return gc->get(i); }

   /** returns the edge info
     * @param i the source edge id
     * @param j the dest. edge id
     * @return the edge info
     */
   ei &get(int i,int j) { return gc->get(i,j); }
   /** returns the edge info
     * @param i the source edge id
     * @param j the dest. edge id
     * @return the edge info
     */
   const ei &get(int i,int j) const { return gc->get(i,j); }
   /** set an edge
     */
   void set(int i,int j, const ei &e) { gc->set(i,j,e);}
   /** set an edge
     */
   void set(const ei &e) { gc->set(e.src(),e.dst(),e);}
   /** set an vertex
     */
   void set(const vi &v) { gc->set(v);}
   /** Display the graph information
     */
   virtual std::ostream &Prt(std::ostream &os) {
      return gc->Prt(os);
   }

};

/** \defgroup pathgroup Path
 * @{
 */
class Path {
   Bob::pvector<int> pi;
   unsigned int nb;
   double cost;
   public:
   /** Constructor
     */
   Path() :pi(),nb(0),cost(0.0) { }
   /** Constructor
     */
   Path(double _c,int o,int i,int l) :pi(3),nb(0),cost(_c) { 
      add(o);add(i);add(l);
   }
   /** Destructor
     */
   virtual ~Path() {}
   /** Add a vertex
     */
   void add(int v) { 
      if ( pi.size()==nb ) pi.resize(nb+10);
      pi[nb++]=v; 
   }
   int get(int i) const { 
      return pi[i]; 
   }
   unsigned int size() const { return nb; }
   void set_cost(double c) { cost=c; }
   double get_cost() { return cost; }
   virtual std::ostream &Prt(std::ostream &os) const {
      os << nb<<" C:"<<cost<<" (";
      for ( unsigned int i=0;i<nb; i++ ) {
         os << pi[i]<<",";
      }
      os<< ")\n";
      return os;
   }
     virtual void Pack(Bob::Serialize &bs) const {
         pi.Pack(bs);
         bs.Pack(&nb,1);
         bs.Pack(&cost,1);
     }
     virtual void UnPack(Bob::DeSerialize &bs) {
         pi.UnPack(bs);
         bs.UnPack(&nb,1);
         bs.UnPack(&cost,1);
     }


};

inline std::ostream &operator<<(std::ostream &os,const Path &c) {
   return c.Prt(os);
}


/** @}
 */

/** \defgroup vertexgroup Vertex
 * @{
 */


/** Vertex
 */
template<class v_info>
class Vertex : public v_info {
   protected:
   int _id;
   public : 
     typedef v_info vertex_info;
     /** Constructor
       */
     Vertex() :v_info(), _id(-1) {} 
     /** Constructor
       */
     Vertex(int _i) :v_info(),_id(_i) {} 
     /** Constructor
       */
     Vertex(int _i,const v_info &vi) :v_info(vi),_id(_i) {} 
     /** Constructor
       */
     Vertex(const Vertex &vi) :v_info(vi),_id(vi._id) {} 
     /** Constructor
       */
     int get_id() const { return _id; }
     /** Constructor
       */
     void set_id(int i) { _id=i; }
     /** the virtual method to display an object on a ostream
       */
     virtual std::ostream &Prt(std::ostream &os) const {
         os << "id:"<<get_id();
         return v_info::Prt(os);
     }
};

template<class v_info>
inline std::ostream &operator<<(std::ostream &os,const Vertex<v_info> &c) {
   return c.Prt(os);
}


/** Coordinate capability to add to the Vertex class
 */
class Coord {
   protected:
   coord_t x,y;
   public : 
     /** Constructor
       */
     Coord() : x(0), y(0) {} 
     /** Constructor
       */
     Coord(coord_t _x,coord_t _y) : x(_x),y(_y) {} 
     /** Constructor
       */
     Coord(const Coord &c) : x(c.x),y(c.y) {} 
     /** returns the x coordinate
       */
     coord_t getx() const { return x; }
     /** returns the x coordinate
       */
     coord_t gety() const { return y; }
     /** sets the x coordinate
       */
     void setx(coord_t _x) { x=_x; }
     /** sets the y coordinate
       */
     void sety(coord_t _y) { y=_y; }
     virtual void Pack(Bob::Serialize &bs) const {
         bs.Pack(&x,1);
         bs.Pack(&y,1);
     }
     virtual void UnPack(Bob::DeSerialize &bs) {
         bs.UnPack(&x,1);
         bs.UnPack(&y,1);
     }
     /** the virtual method to display an object on a ostream
       */
     virtual std::ostream &Prt(std::ostream &os) const {
         return os << '('<<getx()<<','<<gety()<<')';
     }
};

inline std::ostream &operator<<(std::ostream &os,const Coord &c) {
      return c.Prt(os);
}

/** Reduced cost capability to add to the Vertex class
 */
class RedCost {
   protected:
   redcost_t r;
   public : 
     /** Constructor
       */
     RedCost() : r(0.0) {} 
     /** Constructor
       */
     RedCost(redcost_t _r) : r(_r) {} 
     /** Constructor
       */
     RedCost(const RedCost &c) : r(c.r) {} 
     /** returns the x reduced cost
       */
     redcost_t get() const { return r; }
     /** sets the x reduced cost
       */
     void set(redcost_t _r) { r=_r; }
     void Pack(Bob::Serialize &bs) const {
         bs.Pack(&r,1);
     }
     void UnPack(Bob::DeSerialize &bs) {
         bs.UnPack(&r,1);
     }
     /** the virtual method to display an object on a ostream
       */
     virtual std::ostream &Prt(std::ostream &os) const {
         return os << "R("<<get()<<')';
     }
};

inline std::ostream &operator<<(std::ostream &os,const RedCost &c) {
      return c.Prt(os);
}

/** Times capability to add to instanciate the vertex class.
 */
class Times {
   protected:
   timwind_t tb,te,st;
   public : 
     /** Constructor
       */
     Times() :  tb(0), te(0),st(0) {} 
     /** Constructor
       */
     Times(timwind_t _tb,timwind_t _te,timwind_t _st) : tb(_tb),te(_te),st(_st) {} 
     /** Constructor
       */
     Times(const Times &t) : tb(t.tb),te(t.te),st(t.st) {} 
     /** returns the start time of the time window
       */
     timwind_t gettb() const { return tb; }
     /** sets the start time of the time window
       */
     void settb(timwind_t _x) { tb=_x; }
     /** returns the end date of the time window
       */
     timwind_t gette() const { return te; }
     /** sets the end time of the time window
       */
     void sette(timwind_t _y) { te=_y; }
     /** returns the service time
       */
     timwind_t getst() const { return st; }
     /** sets the service time
       */
     void setst(timwind_t _y) { st=_y; }
     virtual void Pack(Bob::Serialize &bs) const {
         bs.Pack(&tb,1);
         bs.Pack(&te,1);
         bs.Pack(&st,1);
     }
     virtual void UnPack(Bob::DeSerialize &bs) {
         bs.UnPack(&tb,1);
         bs.UnPack(&te,1);
         bs.UnPack(&st,1);
     }
     /** the virtual method to display an object on a ostream
       */
     virtual std::ostream &Prt(std::ostream &os) const {
         return os << '('<<gettb()<<','<<gette()<<','<<getst()<<')';
     }
};

inline std::ostream &operator<<(std::ostream &os,const Times &c) {
      return c.Prt(os);
}

/** Cost capability to add to instanciate the vertex class or edge class.
 */
class Cost {
   protected:
   cost_t _c;
   public : 
     /** Constructor
       */
     Cost() : _c(0) {} 
     /** Constructor
       */
     Cost(cost_t _cc) : _c(_cc) {}
     /** Constructor
       */
     Cost(const Cost &t) : _c(t._c) {} 
     /** gets the cost
       */
     cost_t get_cost() const { return _c; }
     /** sets the cost
       */
     void set_cost(cost_t t) { _c=t; }
     virtual void Pack(Bob::Serialize &bs) const {
         bs.Pack(&_c,1);
     }
     virtual void UnPack(Bob::DeSerialize &bs) {
         bs.UnPack(&_c,1);
     }
     /** the virtual method to display an object on a ostream
       */
     virtual std::ostream &Prt(std::ostream &os) const {
         return os << "C:("<<get_cost()<<')';
     }
};

inline std::ostream &operator<<(std::ostream &os,const Cost &c) {
      return c.Prt(os);
}

/** Load capability to add to instanciate the vertex class or edge class.
 */
class Load : public Constraint_value<load_t> {
   public : 
     /** Constructor
       */
     Load() : Constraint_value<load_t>() {} 
     /** Constructor
       */
     Load(load_t _cc) : Constraint_value<load_t>(_cc) {}
     /** Constructor
       */
     Load(const Load &t) : Constraint_value<load_t>(t) {} 
     /** gets the load
       */
     load_t get_load() const { return get(); }
     /** sets the load
       */
     void set_load(load_t _t) { set(_t); }
     /** the virtual method to display an object on a ostream
       */
     virtual std::ostream &Prt(std::ostream &os) const {
         return os << "L:("<<get_load()<<')';
     }
};

inline std::ostream &operator<<(std::ostream &os,const Load &c) {
      return c.Prt(os);
}

/** Dist capability to add to instanciate the vertex class or edge class.
 */
class Dist {
   protected:
   dist_t _c;
   public : 
     /** Constructor
       */
     Dist() : _c(0) {} 
     /** Constructor
       */
     Dist(dist_t _cc) : _c(_cc) {}
     /** Constructor
       */
     Dist(const Dist &t) : _c(t._c) {} 
     /** gets the dist
       */
     dist_t get() const { return _c; }
     /** sets the dist
       */
     void set(dist_t t) { _c=t; }
     virtual void Pack(Bob::Serialize &bs) const {
         bs.Pack(&_c,1);
     }
     virtual void UnPack(Bob::DeSerialize &bs) {
         bs.UnPack(&_c,1);
     }
     /** the virtual method to display an object on a ostream
       */
     virtual std::ostream &Prt(std::ostream &os) const {
         return os << "D:("<<get()<<')';
     }
};

inline std::ostream &operator<<(std::ostream &os,const Dist &c) {
      return c.Prt(os);
}


/** 
 * @}
 */

/** \defgroup edgegroup Edge 
 * @{
 */


/** Simple edge
 */
template<class e_info>
class Edge : public e_info {
   int s,d;
    public:
   typedef e_info edge_info;
   /** Constructor
     */
   Edge() : e_info(),s(-1),d(-1) { }
   /** Constructor
     * @param _s the source vertex id.
     * @param _d the destination  vertex id.
     */
   Edge(int _s,int _d) : e_info(),s(_s),d(_d) { }
   /** Constructor
     * @param _s the source vertex id.
     * @param _d the destination  vertex id.
     */
   Edge(int _s,int _d,const e_info &ei) : e_info(ei),s(_s),d(_d) { }
   /** Constructor
     * @param _s the source vertex id.
     * @param _d the destination  vertex id.
     */
   Edge(const Edge &ei) : e_info(ei),s(ei.s),d(ei.d) { }
   /** isset 
     */
   bool isset() const { return s!=-1; }
   /** get the source
     */
   int src() const { return s; }
   /** get the destination
     */
   int dst() const { return d; }
   virtual void Pack(Bob::Serialize &bs) const {
         bs.Pack(&s,1);
         bs.Pack(&d,1);
         e_info::Pack(bs);
   }
   virtual void UnPack(Bob::DeSerialize &bs) {
         bs.UnPack(&s,1);
         bs.UnPack(&d,1);
         e_info::UnPack(bs);
   }
   /** the virtual method to display an object on a ostream
   */
   virtual std::ostream &Prt(std::ostream &os) const {
         os << "("<<src()<<','<<dst()<<')';
         return e_info::Prt(os);
   }
};

template<class e_info>
inline std::ostream &operator<<(std::ostream &os,const Edge<e_info> &c) {
      return c.Prt(os);
}


/** 
 * @}
 */

/*template <class Graph,class Label, class subState>
class State {
      std::vector< subState > st;
      Graph *g;

    public:
      State(Graph *_g) : st(_g->size()),g(_g) {
          for ( int i=0;i< g->size();i++ ) {
             st[i].resize(g->get(i).const_size());
          }
      }
      virtual ~State() {}
};

template <class Graph,class Label>
class State<Label *> {
      std::vector< Label * > st;
      Graph *g;

    public:
      State(Graph *_g) : st(_g->size()),g(_g) {
          for ( int i=0;i< g->size();i++ ) {
             st[i].resize(g->get(i).const_size());
          }
      }
      virtual ~State() {}
};
*/


typedef Vertex<Load> CVRP_Vertex;
typedef Edge<Dist> CVRP_Edge;
typedef Graph_comp<Load,CVRP_Vertex, CVRP_Edge> CVRP_Graph;
typedef Graph_forb<Load,CVRP_Vertex, CVRP_Edge> FCVRP_Graph;

template <class Graphe, class LP>
void InitLP(Graphe *g,LP *_lp ) {
      std::cout << " Many cases to do\n";
}

template<class LP>
void InitLP(CVRP_Graph *cvrp, LP *lp) {
     
      lp->set_obj_dir( GSP_MIN );
 
      for (int i=1;i<cvrp->size()-1; i++ ) {
         ppglop::row *r=lp->new_row();
         r->set_sense(GSP_LO);
         r->set_rhs(1.0);
      }
      for (int i=1;i<cvrp->size()-1 ; i++ )  {
         ppglop::col *c = lp->new_col();
         c->set_obj((double)(cvrp->get(0,i).get()+cvrp->get(i,cvrp->size()-1).get()));
         c->set_kind(GSP_IV);
         c->set_coeff(i-1,1.0);
         c->set_lobd(0.0); c->set_upbd(1.0);
      }
      lp->set_presolve( 0 );
      lp->set_verbosity( 2 );
      lp->write_lpt( "init_vrp.lp");
      lp->set_class( GSP_LP );
}

void compute_sp(const FCVRP_Graph &cvp, double *rd,std::vector<Path> &pa); 
/** 
 * @}
 */

#endif
