using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CombineCastingTech.Data.Common
{
    public abstract class ItemsDataProvider
    {
        public abstract List<VideoMetaData> GetAll(int max_results);

        public abstract VideoMetaData GetFromID(string ID);

    }
}
