#include "sortrows.h"

sortRows::sortRows()
{

}
MatrixXi sortRows::sort(VectorXd & rmsds)
{
    VectorXd rmsdSort(rmsds.size());
    VectorXi ind(rmsds.size());
    MatrixXi indexSort(rmsds.size(),2);

   igl::sort(rmsds,1,true,rmsdSort, ind);//sort rmsds and put sorted column in rmsdSort.
    // sorted indecies are in ind.

    for(int i=0; i<rmsds.size(); ++i)
    {
        indexSort.col(0).row(i)<<i;
        indexSort.col(1).row(i)=ind.row(i);
    }
  //  cout<<"rmsdSort"<<endl<<rmsdSort<<endl;
    //cout<<"indsort"<<endl<<indexSort<<endl;
    return indexSort;
}
